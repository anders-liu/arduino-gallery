#include "Wireless.h"

#define MAX_PAYLOAD 32
#define RETRY_INTERVAL 77
#define TIMEOUT_CHECK_INTERVAL 1500
#define RTC_TIMEOUT 1500

void Wireless::loopSendThermometerData() {
	if (!ds.f.sensorValueReadyToSend) return;
	if (failedCount > 0 && millis() - lastSendMillis < RETRY_INTERVAL) return;
	if (!radio.isChipConnected()) {
		ds.setWirelessState(S_WL_NOCHIP);
		return;
	}

	WLM_R2C_THERMOMETER msg(ds.sensorResult, ds.temperature, ds.humidity);
	radio.stopListening();
	if (radio.write(&msg, sizeof(msg))) {
		ds.f.sensorValueReadyToSend = 0;
		ds.setWirelessState(S_WL_OK);
		failedCount = 0;
	}
	else {
		radio.flush_tx();
		failedCount++;

		if (failedCount >= 5) {
			ds.f.sensorValueReadyToSend = 0;
			ds.setWirelessState(S_WL_NOSIGNAL);
			failedCount = 0;
		}
	}
	radio.startListening();

	lastSendMillis = millis();
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
		ds.setRtcData(msg->month, msg->day, msg->weekday, msg->hour, msg->minute);
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
