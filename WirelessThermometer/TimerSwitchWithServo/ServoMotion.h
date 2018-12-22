// ServoMotion.h

#ifndef SERVOMOTION_H__
#define SERVOMOTION_H__

#include <arduino.h>
#include <Servo.h>

class ServoMotion {
public:
	ServoMotion(uint8_t controlPin, uint8_t powerPin)
		: controlPin(controlPin), powerPin(powerPin) {}

	void setup();
	void loop();

private:
	uint8_t controlPin;
	uint8_t powerPin;
	Servo servo;

	uint8_t state = 0;
	uint32_t lastMillis = 0;
};

#endif  // SERVOMOTION_H__

