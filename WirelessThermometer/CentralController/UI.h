#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "DataStore.h"

#define NUM_CHANNELS 4

class UI {
public:
	UI(
		byte pinRS, byte pinRW, byte pinE,
		byte pinD4, byte pinD5, byte pinD6, byte pinD7,
		byte pinLed)
		: lcd(pinRS, pinRW, pinE, pinD4, pinD5, pinD6, pinD7)
		, pinLed(pinLed) {
	}

	void setup();
	void loop();

private:
	void loopChannelThemometerData(uint8_t channel, bool forceUpdate);
	void loopChannelUpdateAnimation(uint8_t channel);
	void loopChannelLightData(uint8_t channel, bool forceUpdate);
	void loopRtcData(bool forceUpdate);
	void loopRtcAnimation();

	void clearRow(uint8_t row);
	void writeUnit(uint8_t row);
	void writeThemometerData(uint8_t row, float t, float h);
	void writeRemoteTitle();

	void writeYear(bool show);
	void writeMonth(bool show);
	void writeDay(bool show, uint8_t pos);
	void writeWeekday(bool show);
	void writeHour(bool show);
	void writeMinute(bool show);

private:
	const LiquidCrystal lcd;
	const byte pinLed;
};