#include "ServoMotion.h"
#include "DataStore.h"

#define START_DELAY           200
#define RUNNING_HOLD_PERIOD  1000

enum ServoMotionState_t {
	SS_OFF,
	SS_JUST_ON,
	SS_RUNNING,
};

static void s_powerOff(uint8_t powerPin) {
	digitalWrite(powerPin, LOW);
}

static void s_powerOn(uint8_t powerPin) {
	digitalWrite(powerPin, HIGH);
}

void ServoMotion::setup() {
	pinMode(this->powerPin, OUTPUT);
	s_powerOff(this->powerPin);

	this->servo.attach(this->controlPin);
}

void ServoMotion::loop() {
	switch (this->state) {
	case SS_OFF:
		if (ds.getCurrentServoValueReadyToSend()) {
			this->servo.write(ds.getCurrentServoValue());
			ds.clearCurrentServoValueReadyToSend();
			this->state = SS_JUST_ON;
			this->lastMillis = millis();
		}
		break;

	case SS_JUST_ON:
		if (millis() - this->lastMillis > START_DELAY) {
			s_powerOn(this->powerPin);
			this->state = SS_RUNNING;
			this->lastMillis = millis();
		}
		break;

	case SS_RUNNING:
		if (millis() - this->lastMillis > RUNNING_HOLD_PERIOD) {
			s_powerOff(this->powerPin);
			this->state = SS_OFF;
		}
		break;
	}
}
