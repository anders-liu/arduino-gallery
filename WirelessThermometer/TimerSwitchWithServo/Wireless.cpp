#include "Wireless.h"
#include "DataStore.h"

#define MAX_PAYLOAD 32

#define HEARTBEAT_THERMOMETER_INTERVAL 3000
#define HEARTBEAT_LIGHTDATA_INTERVAL   1343

#define RETRY_INTERVAL 77
#define MAX_RETRY 5

#define SEND_WITH_RETRY(clearSignal, msg, last, count) do { \
	if(this->send(&msg)) { \
		clearSignal(); \
		ds.setWirelessState(WL_S_OK); \
		count = 0; \
	} \
	else { \
		count++; \
		if (ds.getWirelessState() != WL_S_NOCHIP && count >= MAX_RETRY) { \
			clearSignal(); \
			ds.setWirelessState(WS_S_NOSIGNAL); \
			count = 0; \
		} \
	} \
	last = millis(); \
} while(0)

#define NEED_SEND(signal, last, count) ( \
	(signal) || ((count) > 0 && ((millis() - (last)) > RETRY_INTERVAL)) \
)

static bool s_shouldSendThermometerHeartbeat = false;
static void s_clearShouldSendThermometerHeartbeat() {
	s_shouldSendThermometerHeartbeat = false;
}

void Wireless::setup() {
	this->radio.begin();
	this->radio.enableDynamicPayloads();
	this->radio.setCRCLength(RF24_CRC_16);
	this->radio.setPALevel(RF24_PA_LOW);
	this->radio.setAddressWidth(5);
	this->radio.setChannel(this->channel);
	this->radio.openReadingPipe(1, this->rxAddr);
	this->radio.openWritingPipe(this->txAddr);
	this->radio.startListening();
}

void Wireless::loop() {
	this->loopSetHeartbeat();
	this->loopSendThermometerData();
	this->loopSendLightData();
	this->loopReceive();
}

void Wireless::loopSetHeartbeat() {
	if (millis() - this->lastThermometerSendMillis > HEARTBEAT_THERMOMETER_INTERVAL) {
		s_shouldSendThermometerHeartbeat = true;
	}

	if (millis() - this->lastLightDataSendMillis > HEARTBEAT_LIGHTDATA_INTERVAL) {
		ds.setIsPowerOn(ds.getIsPowerOn());
	}
}

void Wireless::loopSendThermometerData() {
	if (NEED_SEND(s_shouldSendThermometerHeartbeat, this->lastThermometerSendMillis, this->retryThermometerSendCount)) {
		WLM_R2C_THERMOMETER msg(0, millis(), 0);
		SEND_WITH_RETRY(s_clearShouldSendThermometerHeartbeat, msg, this->lastThermometerSendMillis, this->retryThermometerSendCount);
	}
}

void Wireless::loopSendLightData() {
	if (NEED_SEND(ds.getIsPowerOnReadyToSend(), this->lastLightDataSendMillis, this->retryLightDataSendCount)) {
		WLM_R2C_LIGHT msg(ds.getIsPowerOn(), 0, 0);
		SEND_WITH_RETRY(ds.clearIsPowerOnReadyToSend, msg, this->lastLightDataSendMillis, this->retryLightDataSendCount);
	}
}

bool Wireless::send(WLM * msg) {
	if (!radio.isChipConnected()) {
		ds.setWirelessState(WL_S_NOCHIP);
		return false;
	}

	radio.stopListening();
	bool r = radio.write(msg, msg->size);
	radio.startListening();

	if (!r) {
		radio.flush_tx();
	}

	return r;
}

void Wireless::loopReceive() {
	if (!radio.available()) {
		radio.printDetails();
		return;
	}

	byte len = radio.getDynamicPayloadSize();
	if (!len) {
		radio.flush_rx();
		return;
	}

	byte payload[MAX_PAYLOAD];
	radio.read(payload, len);

	WLM *baseMsg = reinterpret_cast<WLM*>(payload);

	if (baseMsg->size != len) {
		return;
	}

	switch (baseMsg->type) {
	case MSG_C2R_LIGHT_POWER: {
		WLM_C2R_LIGHT_POWER *msg = (WLM_C2R_LIGHT_POWER *)baseMsg;
		bool isPowerOn = msg->isPowerOn != 0;
		ds.setCurrentServoValue(isPowerOn ? ds.getServoOnValue() : ds.getServoOffValue());
		ds.setIsPowerOn(isPowerOn);
		break;
	}
	}
}
