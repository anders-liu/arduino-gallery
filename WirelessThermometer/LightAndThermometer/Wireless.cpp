#include "Wireless.h"

#define MAX_PAYLOAD 32
#define TIMEOUT_CHECK_INTERVAL 1500
#define RTC_TIMEOUT 1500

void Wireless::loopSendThermometerData() {
	if (ds.f.sensorValueReadyToSend) {
		ds.f.sensorValueReadyToSend = 0;

		WLM_R2C_THERMOMETER msg(ds.sensorResult, ds.temperature, ds.humidity);
		send(&msg);
	}
}

void Wireless::loopSendLightData() {
	if (ds.f.lightValueReadyToSend) {
		ds.f.lightValueReadyToSend = 0;

		WLM_R2C_LIGHT msg(ds.lightIsOn, ds.lightLevel, ds.lightPercentage);
		send(&msg);
	}
}

void Wireless::send(WLM * msg) {
	if (!radio.isChipConnected()) {
		ds.setWirelessState(S_WL_NOCHIP);
		return;
	}

	radio.stopListening();
	if (radio.write(msg, msg->size)) {
		ds.setWirelessState(S_WL_OK);
	}
	else {
		radio.flush_tx();
		ds.setWirelessState(S_WL_NOSIGNAL);
	}
	radio.startListening();
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
