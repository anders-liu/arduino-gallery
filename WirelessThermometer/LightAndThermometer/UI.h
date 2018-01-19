#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#include "DataStore.h"

enum UIDisplayState {
	UI_D_OFF,
	UI_D_TC,
	UI_D_TF,
	UI_D_H,
};

class UI {
public:
	UI(uint8_t addr) : addr(addr) {
	}

	void setup();
	void loop();

	void startSpinner();
	void displayCelsius();
	void displayFahrenheit();
	void displayHumidity();
	void switchUI();

private:
	void loopSpinner();
	void loopAutoOff();
	void loopStatus();
	void loopRtc();

private:
	uint8_t addr;
	Adafruit_SSD1306 display = Adafruit_SSD1306();

	byte spinnerStep = 0;
	unsigned long lastSpinnerMillis = 0;
	UIDisplayState uiState = UI_D_OFF;
	unsigned long lastActiveMillis = 0;
};
