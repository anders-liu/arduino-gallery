#pragma once

#include <Arduino.h>
#include <avr/sleep.h>

typedef void(*SleepEventHandler)(void);

#define SLEEP_INTERVAL 90000

ISR(INT0_vect) {
	// Empty interrupt handler for INT3 which is connect to func button
	// and is used to weak up.
}

ISR(INT1_vect) {
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
		//        7     6 5      4      3      2      1 0
		// PRR0   PRTWI - PRTIM0 -      PRTIM1 PRSPI  - PRADC
		// PRR1   PRUSB - -      PRTIM4 PRTIM3 -      - PRUSART1
		PRR0 = 0b10001101;
#if THIS_APP_DISABLE_USB
		PRR1 = 0b10011000;
#else
		PRR1 = 0b00011000;
#endif

		lastMillis = millis();
	}

	void loop() {
		if (millis() - lastMillis > SLEEP_INTERVAL) {
			if (enterSleepHandler) {
				enterSleepHandler();
			}

			bitSet(EIMSK, INT0);
			bitSet(EIMSK, INT1);
			sleep_mode();
			bitClear(EIMSK, INT1);
			bitClear(EIMSK, INT0);
			refresh();

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

