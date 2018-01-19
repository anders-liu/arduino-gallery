#include "Thermometer.h"

#define THERMOMETER_UPDATE_INTERVAL 4000

void Thermometer::loop() {
	uint32_t ms = millis();

	if (ms - ds.cd[0].thermometerDataUdpatedMillis > THERMOMETER_UPDATE_INTERVAL) {
		uint8_t r = DHT.read(dhtPin);
		if (r == DHTLIB_OK) {
			ds.cd[0].setStatus(CS_OK);
			ds.cd[0].setThermometerData(DHT.temperature, DHT.humidity);
		}
		else {
			ds.cd[0].setStatus(CS_SENSOR_ERROR);
		}
	}
}
