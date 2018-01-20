#include "Wireless.h"

#define MAX_PAYLOAD 32
#define TIMEOUT_CHECK_INTERVAL 1553
#define RTC_TIMEOUT 2200
#define RETRY_INTERVAL 77

#define SEND_WITH_RETRY(signal, msg, last, count) do { \
	if(send(&msg)) { \
		signal = 0; \
		ds.setWirelessState(S_WL_OK); \
		count = 0; \
	} \
	else { \
		count++; \
		if (count >= 5) { \
			signal = 0; \
			ds.setWirelessState(S_WL_NOSIGNAL); \
			count = 0; \
		} \
	} \
	last = millis(); \
} while(0)

#define NEED_SEND(signal, last, count) ( \
	(signal) || ((count) > 0 && ((millis() - (last)) > RETRY_INTERVAL)) \
)

void Wireless::loopSendThermometerData() {
	if (NEED_SEND(ds.f.sensorValueReadyToSend, lastThermometerSendMillis, retryThermometerSendCount)) {
		WLM_R2C_THERMOMETER msg(ds.sensorResult, ds.temperature, ds.humidity);
		SEND_WITH_RETRY(ds.f.sensorValueReadyToSend, msg, lastThermometerSendMillis, retryThermometerSendCount);
	}
}

void Wireless::loopSendLightData() {
	if (NEED_SEND(ds.f.lightValueReadyToSend, lastLightDataSendMillis, retryLightDataSendCount)) {
		WLM_R2C_LIGHT msg(ds.lightIsOn, ds.lightLevel, ds.lightPercentage);
		SEND_WITH_RETRY(ds.f.lightValueReadyToSend, msg, lastLightDataSendMillis, retryLightDataSendCount);
	}
}

bool Wireless::send(WLM * msg) {
	if (!radio.isChipConnected()) {
		ds.setWirelessState(S_WL_NOCHIP);
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
	case MSG_C2R_RTC: {
		WLM_C2R_RTC *msg = (WLM_C2R_RTC *)baseMsg;
		ds.setRtcStatus(S_RTC_OK);
		ds.setRtcData(
			msg->year, msg->month, msg->day, msg->weekday,
			msg->hour, msg->minute);
		break;
	}
	case MSG_C2R_LIGHT_POWER: {
		WLM_C2R_LIGHT_POWER *msg = (WLM_C2R_LIGHT_POWER *)baseMsg;
		ds.lightControlIsOn = msg->isPowerOn;
		ds.f.lightControlIsOnReadyToSet = 1;
		break;
	}
	case MSG_C2R_LIGHT_LEVEL: {
		WLM_C2R_LIGHT_LEVEL *msg = (WLM_C2R_LIGHT_LEVEL *)baseMsg;
		ds.lightControlLevel = msg->level;
		ds.f.lightControlLevelReadyToSet = 1;
		break;
	}
	}
}

void Wireless::loopTimeoutCheck() {
	uint32_t ms = millis();

	if (ms - lastTimeoutCheckMillis < TIMEOUT_CHECK_INTERVAL) {
		return;
	}

	lastTimeoutCheckMillis = ms;

	if (ms - ds.rtcLastUpdatedMillis > RTC_TIMEOUT) {
		ds.setRtcStatus(RTC_TIMEOUT);
	}
}
