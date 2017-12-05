//
// Name:	IrAdjLed.ino
// Created:	11/30/2017 5:11:08 PM
// Author:	Anders Liu
//

#include <Arduino.h>
#include <eeprom.h>
#include <AL_LeonardoRxTxLed.h>
#include "IrController.h"

#define DEBUG 0

#if DEBUG
#define DBG_PRINT(...) Serial.print(__VA_ARGS__)
#define DBG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DBG_PRINT(...)
#define DBG_PRINTLN(...)
#endif // DEBUG

#define PIN_PWM 10
#define PIN_IR 3

AL_LeonardoRxTxLed rxtxled;
IrController ir(PIN_IR);

const byte presetValues[] = {
	0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20,
	22, 24, 26, 28, 30, 34, 38, 42, 46, 54, 62, 70, 80
};

byte pwmLevel = 0;

#define NUM_PWM_LEVEL (sizeof(presetValues)/sizeof(presetValues[0]))

#define ROM_ADDR_PWM_LEVEL 0

bool isPowerOn = true;
bool isPowerSwitchTriggered = false;

unsigned long lastLevelChangedMillis = 0;
bool isLevelChanged = false;

void timerOff() {
	// CS2:0 = 000
	TCCR1B = TCCR1B & 0b11111000;
}

void setupTimer() {
	// Timer1
	OCR1B = presetValues[pwmLevel];  // Default speed.
	ICR1 = 80;  // TOP = 80, PWM = 80*0.5us = 40us, 25kHz, PC fan standard.
				// COM1B1:COM1B0 = 10, output on OC1B, clear on compare match, set on TOP.
	bitSet(TCCR1A, COM1B1);
	bitClear(TCCR1A, COM1B0);
	// WGM13:0 = 1110, fast PWM, TOP=ICR1
	bitSet(TCCR1B, WGM13);
	bitSet(TCCR1B, WGM12);
	bitSet(TCCR1A, WGM11);
	bitClear(TCCR1A, WGM10);

	// CS2:0 = 010, Fclk/8, 2MHz, 0.5us
	TCCR1B = TCCR1B & 0b11111000 | 0b00000010;
}

void switchPower() {
	if (isPowerOn) {
		timerOff();
		digitalWrite(PIN_PWM, LOW);
		isPowerOn = false;
	}
	else {
		setupTimer();
		isPowerOn = true;
	}
}

void setLevel(byte newLevel) {
	if (newLevel >= 0 && newLevel < NUM_PWM_LEVEL) {
		pwmLevel = newLevel;
		OCR1B = presetValues[pwmLevel];
		isLevelChanged = true;
		lastLevelChangedMillis = millis();
	}
}

#if DEBUG
void dbg_printLevel(char leading) {
	DBG_PRINT(leading);
	DBG_PRINT(pwmLevel);
	DBG_PRINT(":");
	DBG_PRINTLN(presetValues[pwmLevel]);
}
#else
#define dbg_printLevel(...)
#endif

void loadSavedPwmLevel() {
	byte savedLevel = EEPROM.read(ROM_ADDR_PWM_LEVEL);
	setLevel(savedLevel);
}

void onKeyDown(IrKey key) {
	switch (key)
	{
	case IRKEY_POWER:
		if (!isPowerSwitchTriggered) {
			isPowerSwitchTriggered = true;
			switchPower();
		}
		break;

	case IRKEY_PLUS:
		if (isPowerOn) {
			setLevel(pwmLevel + 1);
			dbg_printLevel('+');
		}
		break;

	case IRKEY_MINUS:
		if (isPowerOn) {
			setLevel(pwmLevel - 1);
			dbg_printLevel('-');
		}
		break;
	}
}

void onKeyUp(IrKey key) {
	switch (key)
	{
	case IRKEY_POWER:
		isPowerSwitchTriggered = false;
		break;
	}
}

void setup() {
#if DEBUG
	Serial.begin(115200);
#else
	rxtxled.rxOff();
	rxtxled.txOff();
#endif // DEBUG

	pinMode(PIN_PWM, OUTPUT);
	digitalWrite(PIN_PWM, HIGH);
	setupTimer();

	loadSavedPwmLevel();

	ir.handleKeyDown(onKeyDown);
	ir.handleKeyUp(onKeyUp);
	ir.setup();
}

void loop() {
	if (isLevelChanged && (millis() - lastLevelChangedMillis > 3000)) {
		EEPROM.update(ROM_ADDR_PWM_LEVEL, pwmLevel);
		isLevelChanged = false;
		DBG_PRINTLN("Last level saved.");
	}
	ir.loop();
}
