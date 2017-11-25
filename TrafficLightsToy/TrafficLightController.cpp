#include "TrafficLightController.h"

#define S_IDEL 0  /* No use, placeholder. */
#define S_NS_ON 1 /* Green light on north-south way. */
#define S_WE_ON 2 /* Green light on west-east way. */
#define S_NS2WE 3 /* Switching from NS to WE. */
#define S_WE2NS 4 /* Switching from WE to NS. */

#define AUTO_SWITCH_INTERVAL 8000

#define SWITCHING_FLASH_INTERVAL 500
#define SWITCHING_FLASH_COUNT 3

//
// Utilities
//

bool loopTurningRed(TrafficLight l1, TrafficLight l2) {
	static byte step = 0, greenCount = 0, yelloCount = 0;
	static unsigned long lastMills = millis();

	bool done = false;
	unsigned long ms = millis();
	if (ms - lastMills > SWITCHING_FLASH_INTERVAL) {
		lastMills = ms;

		switch (step)
		{
		case 0:
			l1.fullOff();
			l2.fullOff();
			if (greenCount >= SWITCHING_FLASH_COUNT) {
				greenCount = 0;
				step = 2;
			}
			else {
				greenCount++;
				step = 1;
			}
			break;

		case 1:
			l1.greenOn();
			l2.greenOn();
			step = 0;
			break;

		case 2:
			l1.yellowOn();
			l2.yellowOn();
			if (yelloCount >= 3) {
				yelloCount = 0;
				step = 3;
			}
			else {
				yelloCount++;
				step = 2;
			}
			break;

		case 3:
			l1.switchToRed();
			l2.switchToRed();
			step = 4;
			break;

		case 4:
			step = 5;
			break;

		case 5:
			step = 0;
			done = true;
			break;

		default:
			break;
		}
	}

	return done;
}

//
// Member functions
//

void TrafficLightController::setup() {
	northLight.fullOff();
	eastLight.fullOff();
	southLight.fullOff();
	westLight.fullOff();
}

void TrafficLightController::loop() {
	loopSwitching();
	loopAutoSwitch();
}

void TrafficLightController::funcKeyDown() {
	if (!isSleeping) {
		swithLight();
	}
}

void TrafficLightController::funcKeyUp() {
}

void TrafficLightController::enterSleep() {
	northLight.fullOff();
	eastLight.fullOff();
	southLight.fullOff();
	westLight.fullOff();
	isSleeping = true;
}

void TrafficLightController::exitSleep() {
	switch (state) {
	case S_NS_ON:
		northLight.greenOn();
		eastLight.redOn();
		southLight.greenOn();
		westLight.redOn();
		break;

	case S_NS2WE:
		eastLight.redOn();
		westLight.redOn();
		break;

	case S_WE_ON:
		northLight.redOn();
		eastLight.greenOn();
		southLight.redOn();
		westLight.greenOn();
		break;

	case S_WE2NS:
		northLight.redOn();
		southLight.redOn();
		break;
	}

	isSleeping = false;
}

void TrafficLightController::setState(byte newState) {
	switch (newState)
	{
	case S_NS_ON:
		northLight.switchToGreen();
		eastLight.switchToRed();
		southLight.switchToGreen();
		westLight.switchToRed();
		lastOnMs = millis();
		break;

	case S_WE_ON:
		northLight.switchToRed();
		eastLight.switchToGreen();
		southLight.switchToRed();
		westLight.switchToGreen();
		lastOnMs = millis();
		break;

	case S_NS2WE:
	case S_WE2NS:
		break;

	default:
		break;
	}

	state = newState;
}

void TrafficLightController::loopSwitching() {
	switch (state)
	{
	case S_NS_ON:
	case S_WE_ON:
		break;

	case S_NS2WE:
		if (loopTurningRed(northLight, southLight)) {
			setState(S_WE_ON);
		}
		break;

	case S_WE2NS:
		if (loopTurningRed(westLight, eastLight)) {
			setState(S_NS_ON);
		}
		break;

	default:
		setState(S_NS_ON);
		break;
	}
}

void TrafficLightController::loopAutoSwitch() {
	if (millis() - lastOnMs > AUTO_SWITCH_INTERVAL) {
		swithLight();
	}
}

void TrafficLightController::swithLight() {
	switch (state)
	{
	case S_NS_ON:
		setState(S_NS2WE);
		break;

	case S_WE_ON:
		setState(S_WE2NS);
		break;
	}
}
