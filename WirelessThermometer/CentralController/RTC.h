#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "DataStore.h"

#define RTC_FAKE_LOOP 1
#define RTC_INTERVAL 1000

class RTC {
public:
	RTC(uint8_t addr) : addr(addr) {
	}

	void setup();
	void loop();

private:
	void loopSet();
	void loopRead();

private:
	uint8_t addr;
	bool isValueSet = false;
};