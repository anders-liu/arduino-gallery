#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "DataStore.h"

class UI {
public:
	UI(
		byte rsPin, byte rwPin, byte ePin,
		byte d4Pin, byte d5Pin, byte d6Pin, byte d7Pin,
		byte ledPin)
		: lcd(rsPin, rwPin, ePin, d4Pin, d5Pin, d6Pin, d7Pin)
		, ledPin(ledPin) {
	}

public:
	void setup();
	void loop() {
		if (ds.f.updateAnimReadyToShow) {
			ds.f.updateAnimReadyToShow = 0;
			startAnimation();
		}

		loopAnimation();
		loopSensor();
		loopWireless();
		loopRtc();
		loopRtcIndicator();
	}

	void startAnimation() { if (animStep == 0) animStep = 1; }

private:
	void loopSensor();
	void loopWireless();
	void loopAnimation();
	void loopRtc();
	void loopRtcIndicator();

	void writeUnits();
	void writeValue();

	void writeRtc(bool forceWrite);

private:
	LiquidCrystal lcd;
	uint8_t ledPin;

	uint32_t lastAnimMillis = 0;
	uint8_t animStep = 0;

	uint32_t lastRtcIndicatorMillis = 0;
	uint8_t rtcIndicatorStep = 0;
};
