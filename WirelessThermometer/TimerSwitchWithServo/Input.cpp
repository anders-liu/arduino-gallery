#include "Input.h"
#include "DataStore.h"

#define MAX_SERVO_VALUE ((uint8_t)180)
#define MIN_SERVO_VALUE ((uint8_t)0)

#define DOUBLE_CLICK_TIMEOUT    600
#define LONG_PRESS_TIMEOUT     1500

enum RotrySwitchState_t : uint8_t {
	RSS_READY,
	RSS_FIRST_DOWN,
	RSS_FIRST_UP,
	RSS_SECOND_DOWN,
	RSS_SECOND_UP,
	RSS_LONG_PRESS_DOWN,
	RSS_LONG_PRESS_UP,
};

static Input *s_pInput = nullptr;

static void registerEventHandlers()
{
	s_pInput->powerButton.handleKeyDown([] {s_pInput->powerButtonKeyDown(); });
	s_pInput->powerButton.handleKeyUp([] {s_pInput->powerButtonKeyUp(); });
	s_pInput->rotraySwitch.handleKeyDown([] {s_pInput->rotraySwitchKeyDown(); });
	s_pInput->rotraySwitch.handleKeyUp([] {s_pInput->rotraySwitchKeyUp(); });
	s_pInput->rotrayEncoder.handleClockwiseTick([] {s_pInput->rotrayClockwiseTick(); });
	s_pInput->rotrayEncoder.handleCounterclockwiseTick([] {s_pInput->rotrayCounterclockwiseTick(); });
}

void Input::setup() {
	s_pInput = this;
	registerEventHandlers();

	this->powerButton.setup();
	this->rotraySwitch.setup();
	this->rotrayEncoder.setup();
}

void Input::loop() {
	this->powerButton.loop();
	this->rotraySwitch.loop();
	this->rotrayEncoder.loop();

	uint32_t ms = millis();
	switch (this->rotraySwitchState) {
	case RSS_READY:
	case RSS_LONG_PRESS_DOWN:
		break;

	case RSS_FIRST_DOWN:
		if (ms - this->rotrySwitchLastMillis > LONG_PRESS_TIMEOUT) {
			this->rotraySwitchState = RSS_LONG_PRESS_DOWN;
		}
		break;

	case RSS_SECOND_UP:
		this->rotraySwitchState = RSS_READY;
		this->onRotraySwitchDoubleClicked();
		break;

	case RSS_LONG_PRESS_UP:
		this->rotraySwitchState = RSS_READY;
		this->onRotraySwitchLongPressed();
		break;

	default:
		if (ms - this->rotrySwitchLastMillis > DOUBLE_CLICK_TIMEOUT) {
			this->rotraySwitchState = RSS_READY;
		}
		break;
	}
}

void Input::powerButtonKeyDown() {
	switch (ds.getUiStage()) {
	case UI_STAGE_RUNNING:
	{
		bool shouldPowerOn = !ds.getIsPowerOn();
		ds.setCurrentServoValue(shouldPowerOn ? ds.getServoOnValue() : ds.getServoOffValue());
		ds.setIsPowerOn(shouldPowerOn);
	}
	break;
	}
}

void Input::powerButtonKeyUp() {
}

void Input::rotraySwitchKeyDown() {
	switch (this->rotraySwitchState) {
	case RSS_READY:
		this->rotraySwitchState = RSS_FIRST_DOWN;
		this->rotrySwitchLastMillis = millis();
		break;

	case RSS_FIRST_UP:
		this->rotraySwitchState = RSS_SECOND_DOWN;
		break;
	}
}

void Input::rotraySwitchKeyUp() {
	switch (this->rotraySwitchState) {
	case RSS_FIRST_DOWN:
		this->rotraySwitchState = RSS_FIRST_UP;
		break;

	case RSS_SECOND_DOWN:
		this->rotraySwitchState = RSS_SECOND_UP;
		break;

	case RSS_LONG_PRESS_DOWN:
		this->rotraySwitchState = RSS_LONG_PRESS_UP;
		break;
	}
}

void Input::rotrayClockwiseTick() {
	uint8_t value;

	switch (ds.getUiStage()) {
	case UI_STAGE_CALIBRATE_OFF_VALUE:
		value = ds.getCalibrationOffValue();
		if (value < MAX_SERVO_VALUE) {
			ds.setCalibrationOffValue(value + 1);
		}
		break;

	case UI_STAGE_CALIBRATE_ON_VALUE:
		value = ds.getCalibrationOnValue();
		if (value < MAX_SERVO_VALUE) {
			ds.setCalibrationOnValue(value + 1);
		}
		break;
	}
}

void Input::rotrayCounterclockwiseTick() {
	uint8_t value;

	switch (ds.getUiStage()) {
	case UI_STAGE_CALIBRATE_OFF_VALUE:
		value = ds.getCalibrationOffValue();
		if (value > MIN_SERVO_VALUE) {
			ds.setCalibrationOffValue(value - 1);
		}
		break;

	case UI_STAGE_CALIBRATE_ON_VALUE:
		value = ds.getCalibrationOnValue();
		if (value > MIN_SERVO_VALUE) {
			ds.setCalibrationOnValue(value - 1);
		}
		break;
	}
}

void Input::onRotraySwitchDoubleClicked() {
	switch (ds.getUiStage()) {
	case UI_STAGE_RUNNING:
		ds.setUiStage(UI_STAGE_CALIBRATE_OFF_VALUE);
		break;

	case UI_STAGE_CALIBRATE_OFF_VALUE:
		ds.setCalibrationOffValue(ds.getServoOffValue());
		ds.setUiStage(UI_STAGE_CALIBRATE_ON_VALUE);
		break;

	case UI_STAGE_CALIBRATE_ON_VALUE:
		ds.setCalibrationOnValue(ds.getServoOnValue());
		ds.setUiStage(UI_STAGE_RUNNING);
		break;
	}
}

void Input::onRotraySwitchLongPressed() {
	switch (ds.getUiStage()) {
	case UI_STAGE_CALIBRATE_OFF_VALUE:
		ds.setServoOffValue(ds.getCalibrationOffValue());
		ds.setCurrentServoValue(ds.getCalibrationOffValue());
		break;

	case UI_STAGE_CALIBRATE_ON_VALUE:
		ds.setServoOnValue(ds.getCalibrationOnValue());
		ds.setCurrentServoValue(ds.getCalibrationOnValue());
		break;
	}
}
