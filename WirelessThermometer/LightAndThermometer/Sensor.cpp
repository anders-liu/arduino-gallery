#include "Sensor.h"

#define UPDATE_INTERVAL 4147

void Sensor::loop() {
	uint32_t ms = millis();
	if (ms - lastUpdateMillis > UPDATE_INTERVAL) {
		lastUpdateMillis = ms;

		uint8_t r = DHT.read22(pin);

		if (r == DHTLIB_OK) {
			ds.setSensorState(S_S_OK);
			ds.setSensorResult(r, DHT.temperature, DHT.humidity);
		}
		else {
			ds.setSensorState(S_S_ERROR);
			ds.setSensorResult(r);
		}
	}
}
