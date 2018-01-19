#pragma once

#include <Arduino.h>
#include "DataStore.h"

class NightLight {
public:
	NightLight() {
	}

	void setup();
	void loop();

	void switchPower();
	void plus();
	void minus();

	void setPower(bool on);
	void setLevel(byte newLevel);

private:
	uint32_t lastSetSendMillis = 0;
};