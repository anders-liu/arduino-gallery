#pragma once

#include <Arduino.h>
#include <avr/sleep.h>

typedef void(*SleepEventHandler)(void);

#define SLEEP_INTERVAL 90000

ISR(INT3_vect) {
	// Empty interrupt handler for INT3 which is connect to func button
	// and is used to weak up.
}

class PowerManager
{
public:
	PowerManager() {}

	void setup() {
		// Set sleep mode to Power-Down.
		// SM2:0 = 010
		bitClear(SMCR, SM2);
		bitSet(SMCR, SM1);
		bitClear(SMCR, SM0);

		// Turn off unused modules.
		PRR0 = 0b10001101;
		PRR1 = 0b10011001;

		lastMillis = millis();
	}

	void loop() {
		if (millis() - lastMillis > SLEEP_INTERVAL) {
			if (enterSleepHandler) {
				enterSleepHandler();
			}

			bitSet(EIMSK, INT3);
			sleep_mode();
			bitClear(EIMSK, INT3);

			if (exitSleepHandler) {
				exitSleepHandler();
			}
		}
	}

	void refresh() { lastMillis = millis(); }

	void handleEnterSleep(SleepEventHandler handler) { enterSleepHandler = handler; }
	void handleExitSleep(SleepEventHandler handler) { exitSleepHandler = handler; }

private:
	SleepEventHandler enterSleepHandler = nullptr, exitSleepHandler = nullptr;
	unsigned long lastMillis = 0;
};

