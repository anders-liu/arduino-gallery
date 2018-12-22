// Input.h

#ifndef INPUT_H__
#define INPUT_H__

#include <Arduino.h>

#include <AL_PushButton.h>
#include <AL_RotrayEncoder.h>

class Input {
public:
	Input(uint8_t powerButtonPin, uint8_t rotraySwitchPin, uint8_t rotrayAPin, uint8_t rotrayBPin)
		: powerButton(powerButtonPin), rotraySwitch(rotraySwitchPin), rotrayEncoder(rotrayAPin, rotrayBPin) {}

	void setup();
	void loop();

private:
	AL_PushButton powerButton;
	AL_PushButton rotraySwitch;
	AL_RotrayEncoder rotrayEncoder;

	uint8_t rotraySwitchState = 0;
	uint32_t rotrySwitchLastMillis = 0;

	void onRotraySwitchDoubleClicked();
	void onRotraySwitchLongPressed();

	void powerButtonKeyDown();
	void powerButtonKeyUp();
	void rotraySwitchKeyDown();
	void rotraySwitchKeyUp();
	void rotrayClockwiseTick();
	void rotrayCounterclockwiseTick();

	friend void registerEventHandlers();
};

#endif  // INPUT_H__

