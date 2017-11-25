#pragma once

#include <Arduino.h>

#include "TrafficLight.h"

class TrafficLightController
{
public:
	TrafficLightController(
		const TrafficLight& northLight,
		const TrafficLight& eastLight,
		const TrafficLight& southLight,
		const TrafficLight& westLight)
		: northLight(northLight),
		eastLight(eastLight),
		southLight(southLight),
		westLight(westLight) {
	}

	void setup();
	void loop();

	void funcKeyDown();
	void funcKeyUp();

	void enterSleep();
	void exitSleep();

private:
	const TrafficLight &northLight, &eastLight, &southLight, &westLight;

	byte /* S_xxx */ state = 0;  // Current running state.
	unsigned long lastOnMs = 0;
	bool isSleeping = false;

	void setState(byte /* S_xxx */ newState);
	void loopSwitching();
	void loopAutoSwitch();
	void swithLight();
};

