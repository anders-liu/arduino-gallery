#include "Wireless.h"

#define NO_SIGNAL_TIMEOUT 5000
#define LIGHT_DATA_TIMEOUT 12000
#define NO_SIGNAL_CHECK_INTERVAL 1000
#define MAX_PAYLOAD 32

#define TX_ADDR(idx) ((const byte *)txPipeAddrs + (idx) * WL_ADDR_WIDTH)
#define RX_ADDR(idx) ((const byte *)rxPipeAddrs + (idx) * WL_ADDR_WIDTH)

void Wireless::setup() {
	rf.begin();
	rf.enableDynamicPayloads();
	rf.setCRCLength(RF24_CRC_16);
	rf.setPALevel(RF24_PA_LOW);
	rf.setAddressWidth(WL_ADDR_WIDTH);
	rf.setChannel(channel);
	rf.setRetries(15, 15);
	for (byte i = 0; i < WL_MAX_REMOTE; i++) {
		rf.openReadingPipe(i + 1, RX_ADDR(i));
	}
	rf.startListening();
}

void Wireless::loopReceive() {
	byte pipe = 0;
	if (!rf.available(&pipe)) {
		return;
	}

	if (pipe == 0 || pipe > WL_MAX_REMOTE) {
		rf.flush_rx();
		rf.flush_tx();
		return;
	}

	byte len = rf.getDynamicPayloadSize();
	if (!len) {
		rf.flush_rx();
		rf.flush_tx();
		return;
	}

	byte payload[MAX_PAYLOAD];
	rf.read(payload, len);

	WLM *baseMsg = reinterpret_cast<WLM*>(payload);
	if (baseMsg->size != len) {
		return;
	}

	switch (baseMsg->type) {
	case MSG_R2C_THERMOMETER: {
		WLM_R2C_THERMOMETER *msg = reinterpret_cast<WLM_R2C_THERMOMETER*>(baseMsg);
		if (msg->sensorStatus != 0 /* DHTLIB_OK */) {
			ds.cd[pipe].setStatus(CS_SENSOR_ERROR);
			ds.cd[pipe].setThermometerData(INVALID_TEMPERATURE, INVALID_HUMIDITY);
		}
		else {
			float t = (float)msg->temperature10Int / 10;
			float h = (float)msg->humidity10Int / 10;
			ds.cd[pipe].setStatus(CS_OK);
			ds.cd[pipe].setThermometerData(t, h);
		}
		break;
	}

	case MSG_R2C_LIGHT: {
		WLM_R2C_LIGHT *msg = reinterpret_cast<WLM_R2C_LIGHT*>(baseMsg);
		bool isPowerOn = WLM_LIGHT_IS_POWER_ON(msg->status);
		byte level = WLM_LIGHT_LEVEL(msg->status);
		float percentage = (float)msg->percentage10Int / 10;
		ds.cd[pipe].setLightStatus(CS_LIGHT_OK);
		ds.cd[pipe].setLightPower(isPowerOn);
		ds.cd[pipe].setLightLevel(level, percentage);
		break;
	}
	}
}

void Wireless::loopNoSignal() {
	uint32_t ms = millis();

	if (ms - lastNoSignalCheckMillis > NO_SIGNAL_CHECK_INTERVAL) {
		lastNoSignalCheckMillis = ms;

		for (uint8_t i = 0; i < WL_MAX_REMOTE; i++) {
			ChannelData &cd = ds.cd[i + 1];
			if (ms - cd.thermometerDataUdpatedMillis > NO_SIGNAL_TIMEOUT) {
				cd.setStatus(CS_NO_SIGNAL);
			}
			if (ms - cd.lightDataUpdatedMillis > LIGHT_DATA_TIMEOUT) {
				cd.setLightStatus(CS_LIGHT_TIMEOUT);
			}
		}
	}
}

void Wireless::loopSendRtc() {
	if (!ds.f.rtcDataReadyToSend) {
		return;
	}

	ds.f.rtcDataReadyToSend = 0;

	WLM_C2R_RTC msg(
		ds.rtcYear, ds.rtcMonth, ds.rtcDay, ds.rtcWeekday,
		ds.rtcHour, ds.rtcMinute, ds.rtcSecond, false, false);

	rf.stopListening();
	for (uint8_t i = 0; i < WL_MAX_REMOTE; i++) {
		rf.openWritingPipe(TX_ADDR(i));
		rf.write(&msg, sizeof(msg));
	}
	rf.startListening();
}

void Wireless::loopSendLightControl() {
	for (uint8_t i = 1; i < WL_MAX_REMOTE; i++) {
		ChannelData &cd = ds.cd[i + 1];

		if (cd.f.lightPowerControlReadyToSend) {
			cd.f.lightPowerControlReadyToSend = 0;
			WLM_C2R_LIGHT_POWER powerMsg(cd.lightControlIsOn);
			rf.stopListening();
			rf.openWritingPipe(TX_ADDR(i));
			rf.write(&powerMsg, sizeof(powerMsg));
			rf.startListening();
		}

		if (cd.f.lightLevelControlReadyToSend) {
			cd.f.lightLevelControlReadyToSend = 0;
			WLM_C2R_LIGHT_LEVEL levelMsg(cd.lightControlLevel);
			rf.stopListening();
			rf.openWritingPipe(TX_ADDR(i));
			rf.write(&levelMsg, sizeof(levelMsg));
			rf.startListening();
		}
	}
}
