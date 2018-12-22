#include "UI.h"
#include "DataStore.h"
#include <AL_LeonardoRxTxLed.h>

#define CALIBRATE_BLINK_INTERVAL_MS  500

static AL_LeonardoRxTxLed s_rxtx;

static void s_formatInt(uint8_t v, char *buf) {
	if (v < 10) {
		buf[0] = ' ';
		buf[1] = ' ';
		buf[2] = '0' + v;
	}
	else if (v < 100) {
		buf[0] = ' ';
		buf[1] = '0' + v / 10;
		buf[2] = '0' + v % 10;
	}
	else {
		buf[0] = '0' + v / 100;
		uint8_t r = v % 100;
		buf[1] = '0' + r / 10;
		buf[2] = '0' + r % 10;
	}

	buf[3] = 0;
}

static void s_printCalibrateOnOffTitle(LiquidCrystal_I2C &lcd) {
	lcd.setCursor(0, 1);
	lcd.print("OFF:");
	lcd.setCursor(10, 1);
	lcd.print("ON:");
}

static void s_printInt(LiquidCrystal_I2C &lcd, uint8_t v) {
	char vs[4];
	s_formatInt(v, vs);
	lcd.print(vs);
}

static void s_printCalibrateOffValue(LiquidCrystal_I2C &lcd, uint8_t v) {
	lcd.setCursor(4, 1);
	s_printInt(lcd, v);
}

static void s_printCalibrateOnValue(LiquidCrystal_I2C &lcd, uint8_t v) {
	lcd.setCursor(13, 1);
	s_printInt(lcd, v);
}

void UI::setup() {
	pinMode(this->powerLedPin, OUTPUT);
	s_rxtx.rxOff();
	s_rxtx.txOff();

	this->lcd.init();
	this->lcd.backlight();
}

void UI::loop() {
	if (ds.getUiStageChanged()) {
		this->loopUIStageChanged();
		ds.clearUiStageChanged();
	}
	else {
		switch (ds.getUiStage()) {
		case UI_STAGE_RUNNING:             this->loopRunning();           break;
		case UI_STAGE_CALIBRATE_OFF_VALUE: this->loopCalibrateOffValue(); break;
		case UI_STAGE_CALIBRATE_ON_VALUE:  this->loopCalibrateOnValue();  break;
		}
	}

	if (ds.getIsPowerOnReadyToShow()) {
		digitalWrite(this->powerLedPin, ds.getIsPowerOn() ? LOW : HIGH);
		ds.clearIsPowerOnReadyToShow();
	}
}

void UI::loopUIStageChanged() {
	this->lcd.clear();
	this->lcd.setCursor(0, 0);

	switch (ds.getUiStage()) {
	case UI_STAGE_RUNNING:
		this->lcd.print("Running");
		break;

	case UI_STAGE_CALIBRATE_OFF_VALUE:
		this->lcd.print("Set OFF value");
		s_printCalibrateOnOffTitle(this->lcd);
		s_printCalibrateOffValue(this->lcd, ds.getCalibrationOffValue());
		s_printCalibrateOnValue(this->lcd, ds.getCalibrationOnValue());
		this->lastAnimationMillis = millis();
		this->calibrationBlinkOn = true;
		break;

	case UI_STAGE_CALIBRATE_ON_VALUE:
		this->lcd.print("Set ON value");
		s_printCalibrateOnOffTitle(this->lcd);
		s_printCalibrateOffValue(this->lcd, ds.getCalibrationOffValue());
		s_printCalibrateOnValue(this->lcd, ds.getCalibrationOnValue());
		this->lastAnimationMillis = millis();
		this->calibrationBlinkOn = true;
		break;
	}
}

void UI::loopRunning() {
}

void UI::loopCalibrateOffValue() {
	if (ds.getCalibrationOffValueReadyToShow()) {
		s_printCalibrateOffValue(this->lcd, ds.getCalibrationOffValue());
		ds.clearCalibrationOffValueReadyToShow();
	}
	else {
		this->loopCalibrateBlinkAnimation(true);
	}
}

void UI::loopCalibrateOnValue() {
	if (ds.getCalibrationOnValueReadyToShow()) {
		s_printCalibrateOnValue(this->lcd, ds.getCalibrationOnValue());
		ds.clearCalibrationOnValueReadyToShow();
	}
	else {
		this->loopCalibrateBlinkAnimation(false);
	}
}

void UI::loopCalibrateBlinkAnimation(bool forOff) {
	if (millis() - this->lastAnimationMillis > CALIBRATE_BLINK_INTERVAL_MS) {
		this->lcd.setCursor(forOff ? 3 : 12, 1);
		this->lcd.write(this->calibrationBlinkOn ? ' ' : ':');
		this->calibrationBlinkOn = !this->calibrationBlinkOn;
		this->lastAnimationMillis = millis();
	}
}
