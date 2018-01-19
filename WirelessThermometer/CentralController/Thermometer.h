// Thermometer.h

#pragma once

#include <Arduino.h>
#include <dht.h>
#include "DataStore.h"

class Thermometer {
public:
	Thermometer(uint8_t dhtPin)
		: dhtPin(dhtPin) {
	}

	void setup() {}
	void loop();

private:
	uint8_t dhtPin;
	dht DHT;
};
