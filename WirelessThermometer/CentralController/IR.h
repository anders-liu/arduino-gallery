#pragma once

#include <Arduino.h>
#include <IRremote.h>
#include "DataStore.h"

enum IRKey {
	IRKEY_UNKNOW,
	IRKEY_LEFT,
	IRKEY_RIGHT,
	IRKEY_UP,
	IRKEY_DOWN,
	IRKEY_OK,
	IRKEY_A,
	IRKEY_B,
	IRKEY_C,
	IRKEY_D,
	IRKEY_TEST,
};

class IR {
public:
	IR(uint8_t recvpin)
		: irRecv(recvpin) {
	}

	void setup();
	void loop();

private:
	void onKeyDown(IRKey key);
	void enterRtcEditing();
	void switchRtcEditingInc();
	void switchRtcEditingDec();
	void rtcIncrease();
	void rtcDecrease();
	void lightControlSwitchPower(uint8_t channel);
	void lightControlLevelIncrease(uint8_t channel);
	void lightControlLevelDecrease(uint8_t channel);

private:
	IRrecv irRecv;
	uint32_t lastMillis = 0;
	uint32_t lastCodeValue = 0;
};

