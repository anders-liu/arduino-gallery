////////////////////////////////////////////////////////////////////////////////
//
// TimerSwitchWithServo
//
// Using servo to turn on/off switch for Christmas LED lights.
//
////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "DataStore.h"
#include "Input.h"
#include "UI.h"
#include "ServoMotion.h"
#include "Wireless.h"

#define PIN_POWER_LED               4
#define PIN_POWER_BUTTON            5

#define PIN_SERVO_POWER            10
#define PIN_SERVO_CONTROL           9

#define PIN_RF24_CE                A0
#define PIN_RF24_CSN               A1

#define PIN_ROTRAY_SW               6
#define PIN_ROTRAY_A                7
#define PIN_ROTRAY_B                8

#define LCD_ADDR                 0x3F

#define WL_CHANNEL ((uint8_t)6)

static const byte rxPipeAddr[] = { 0xB5, 0xDB, 0xBD, 0xEC, 0xCE };
static const byte txPipeAddr[] = { 0xB5, 0xCE, 0xEC, 0xBD, 0xDB };

UI ui(LCD_ADDR, PIN_POWER_LED);
Input input(PIN_POWER_BUTTON, PIN_ROTRAY_SW, PIN_ROTRAY_A, PIN_ROTRAY_B);
ServoMotion servoMotion(PIN_SERVO_CONTROL, PIN_SERVO_POWER);
Wireless wl(PIN_RF24_CE, PIN_RF24_CSN, WL_CHANNEL, txPipeAddr, rxPipeAddr);

bool isCalibrator = false;

void checkCalibarator() {
	pinMode(PIN_ROTRAY_SW, INPUT_PULLUP);
	if (!digitalRead(PIN_ROTRAY_SW)) {
		isCalibrator = true;
		delay(100);
	}

	if (isCalibrator && !digitalRead(PIN_ROTRAY_SW)) {
		delay(1000);
	}

	if (isCalibrator && !digitalRead(PIN_ROTRAY_SW)) {
		return;
	}

	isCalibrator = false;
	return;
}

void setup() {
	Serial.begin(115200);

	checkCalibarator();

	ds.setup();
	ui.setup(isCalibrator);
	input.setup();
	servoMotion.setup();

	if (!isCalibrator) {
		wl.setup();
	}
}

void loop() {
	input.loop();
	ds.loop();
	ui.loop(isCalibrator);
	servoMotion.loop();

	if (!isCalibrator) {
		wl.loop();
	}
}
