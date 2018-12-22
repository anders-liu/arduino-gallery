////////////////////////////////////////////////////////////////////////////////
//
// TimerSwitchWithServo
//
// Using servo to turn on/off switch for Christmas LED lights.
//
////////////////////////////////////////////////////////////////////////////////

#include "DataStore.h"
#include "Input.h"
#include "UI.h"
#include "ServoMotion.h"

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

UI ui(LCD_ADDR, PIN_POWER_LED);
Input input(PIN_POWER_BUTTON, PIN_ROTRAY_SW, PIN_ROTRAY_A, PIN_ROTRAY_B);
ServoMotion servoMotion(PIN_SERVO_CONTROL, PIN_SERVO_POWER);

void setup() {
	ds.setup();
	ui.setup();
	input.setup();
	servoMotion.setup();
}

void loop() {
	input.loop();
	ui.loop();
	ds.loop();
	servoMotion.loop();
}
