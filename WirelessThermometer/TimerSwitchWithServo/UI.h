// UI.h

#ifndef UI_H__
#define UI_H__

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class UI {
public:
	UI(uint8_t lcdAddr, uint8_t powerLedPin)
		: lcd(lcdAddr, 16, 2), powerLedPin(powerLedPin) {}

	void setup(bool isCalibarator);
	void loop(bool isCalibarator);

private:
	LiquidCrystal_I2C lcd;
	uint8_t powerLedPin;

	uint32_t lastAnimationMillis = 0;
	bool calibrationBlinkOn = true;

	void loopUIStageChanged();
	void loopRunning();
	void loopCalibrateOffValue();
	void loopCalibrateOnValue();
	void loopCalibrateBlinkAnimation(bool forOff);
};

#endif  // UI_H__

