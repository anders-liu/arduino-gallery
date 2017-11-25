//
// Name:	TrafficLightsToy.ino
// Created:	11/24/2017 4:07:36 AM
// Author:	Anders Liu
//

#include <Arduino.h>
#include <AL_LeonardoRxTxLed.h>
#include <AL_PushButton.h>

#include "TrafficLight.h"
#include "TrafficLightController.h"
#include "PowerManager.h"

//
// Wiring
//

#define LED_WR_PIN 4
#define LED_WY_PIN 5
#define LED_WG_PIN 6

#define LED_NR_PIN 7
#define LED_NY_PIN 8
#define LED_NG_PIN 9

#define LED_ER_PIN 16
#define LED_EY_PIN 14
#define LED_EG_PIN 15

#define LED_SR_PIN 18
#define LED_SY_PIN 19
#define LED_SG_PIN 20

#define BTN_FUNC_PIN 1

//
// Parts
//

AL_LeonardoRxTxLed rxtx;
AL_PushButton btnFunc(BTN_FUNC_PIN);
TrafficLight northLight(LED_NR_PIN, LED_NY_PIN, LED_NG_PIN);
TrafficLight eastLight(LED_ER_PIN, LED_EY_PIN, LED_EG_PIN);
TrafficLight southLight(LED_SR_PIN, LED_SY_PIN, LED_SG_PIN);
TrafficLight westLight(LED_WR_PIN, LED_WY_PIN, LED_WG_PIN);
TrafficLightController controller(northLight, eastLight, southLight, westLight);
PowerManager pm;

//
// Event Handlers
//

void onBtnFuncKeyDown() {
	controller.funcKeyDown();
	pm.refresh();
}

void onBtnFuncKeyUp() {
	controller.funcKeyUp();
}

void onEnteringSleep() {
	controller.enterSleep();
}

void onExitSleep() {
	controller.exitSleep();
}

//
// Main functions
//

void setup() {
	// Turn off rx/tx led for power saving.
	rxtx.rxOff();
	rxtx.txOff();

	controller.setup();

	btnFunc.handleKeyDown(onBtnFuncKeyDown);
	btnFunc.handleKeyUp(onBtnFuncKeyUp);
	btnFunc.setup();

	pm.handleEnterSleep(onEnteringSleep);
	pm.handleExitSleep(onExitSleep);
	pm.setup();
}

void loop() {
	btnFunc.loop();
	controller.loop();
	pm.loop();
}
