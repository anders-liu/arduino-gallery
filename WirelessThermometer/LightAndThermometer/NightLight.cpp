#include <EEPROM.h>
#include "NightLight.h"

#define PIN_PWM 10

static const byte presetValues[] = {
	0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20,
	22, 24, 26, 28, 30, 34, 38, 42, 46, 54, 62, 70, 80
};

#define NUM_PWM_LEVEL (sizeof(presetValues)/sizeof(presetValues[0]))
#define ROM_ADDR_PWM_LEVEL 0

#define FORCE_SEND_INTERVAL 1533

//
// Utilities
//

void setupTimer(byte initLevel) {
	// Timer1
	OCR1B = presetValues[initLevel];  // Default speed.
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
	bitClear(TCCR1B, CS12);
	bitSet(TCCR1B, CS11);
	bitClear(TCCR1B, CS10);
}

void timerOff() {
	// CS2:0 = 000
	bitClear(TCCR1B, CS12);
	bitClear(TCCR1B, CS11);
	bitClear(TCCR1B, CS10);
}

//
// Implementations
//

void NightLight::setup() {
	pinMode(PIN_PWM, OUTPUT);
	digitalWrite(PIN_PWM, HIGH);

	setPower(true);

	// Load saved level;
	byte savedLevel = EEPROM.read(ROM_ADDR_PWM_LEVEL);
	setLevel(savedLevel);
}

void NightLight::loop() {
	if (ds.f.lightControlLevelReadyToSet) {
		ds.f.lightControlLevelReadyToSet = 0;

		uint8_t newLevel = ds.lightControlLevel;
		if (newLevel >= NUM_PWM_LEVEL) {
			newLevel = NUM_PWM_LEVEL - 1;
		}
		setLevel(ds.lightControlLevel);
	}

	if (ds.f.lightControlIsOnReadyToSet) {
		ds.f.lightControlIsOnReadyToSet = 0;
		setPower(ds.lightControlIsOn);
	}

	// Force to send a light value even no change.
	if (millis() - lastSetSendMillis > FORCE_SEND_INTERVAL) {
		ds.f.lightValueReadyToSend = 1;
		lastSetSendMillis = millis();
	}
}

void NightLight::switchPower() {
	setPower(!ds.lightIsOn);
}

void NightLight::plus() {
	setLevel(ds.lightLevel + 1);
}

void NightLight::minus() {
	setLevel(ds.lightLevel - 1);
}

void NightLight::setPower(bool on) {
	if (on) {
		setupTimer(ds.lightLevel);
	}
	else {
		timerOff();
		digitalWrite(PIN_PWM, LOW);
		EEPROM.update(ROM_ADDR_PWM_LEVEL, ds.lightLevel);
	}
	ds.lightIsOn = on;
	ds.f.lightValueReadyToSend = 1;
	lastSetSendMillis = millis();
}

void NightLight::setLevel(byte newLevel) {
	if (ds.lightIsOn && newLevel >= 0 && newLevel < NUM_PWM_LEVEL) {
		ds.lightLevel = newLevel;
		ds.lightPercentage = (float)presetValues[ds.lightLevel] * 100.0f / 80;
		OCR1B = presetValues[ds.lightLevel];
		ds.f.lightValueReadyToSend = 1;
		lastSetSendMillis = millis();
	}
}
