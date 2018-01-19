#pragma once

#include <Arduino.h>
#include <dht.h>

#include "DataStore.h"

class Sensor {
public:
	Sensor(uint8_t pin)
		: pin(pin) {
	}

public:
	void setup() {}
	void loop();

private:
	dht DHT;
	uint8_t pin;
	uint32_t lastUpdateMillis = 0;
};
