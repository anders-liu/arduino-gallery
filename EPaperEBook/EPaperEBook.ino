#include <Arduino.h>
#include <AL_LeonardoRxTxLed.h>
#include <AL_PushButton.h>

#include "def.h"
#include "PowerManager.h"
#include "UI.h"

AL_LeonardoRxTxLed rxtx;
PowerManager pm;
AL_PushButton btnR(PIN_BTN_R);
AL_PushButton btnL(PIN_BTN_L);

UI ui;

void onRKeyDown() {
#if THIS_APP_USE_SERIAL_DEBUG
	Serial.println("main.onRKeyDown");
#endif
	pm.refresh();
	ui.input(UI_I_RD);
#if THIS_APP_USE_SERIAL_DEBUG
	Serial.println("----");
#endif
}

void onRKeyUp() {
#if THIS_APP_USE_SERIAL_DEBUG
	Serial.println("main.onRKeyUp");
#endif
	pm.refresh();
	ui.input(UI_I_RU);
#if THIS_APP_USE_SERIAL_DEBUG
	Serial.println("----");
#endif
}

void onLKeyDown() {
#if THIS_APP_USE_SERIAL_DEBUG
	Serial.println("main.onLKeyDown");
#endif
	pm.refresh();
	ui.input(UI_I_LD);
#if THIS_APP_USE_SERIAL_DEBUG
	Serial.println("----");
#endif
}

void onLKeyUp() {
#if THIS_APP_USE_SERIAL_DEBUG
	Serial.println("main.onLKeyUp");
#endif
	pm.refresh();
	ui.input(UI_I_LU);
#if THIS_APP_USE_SERIAL_DEBUG
	Serial.println("----");
#endif
}

void setup() {
	rxtx.rxOff();
	rxtx.txOff();
	pm.setup();

	Serial.begin(115200);
	Serial1.begin(115200);

	ui.setup(0, 0);

	btnR.handleKeyDown(onRKeyDown);
	btnR.handleKeyUp(onRKeyUp);
	btnR.setup();

	btnL.handleKeyDown(onLKeyDown);
	btnL.handleKeyUp(onLKeyUp);
	btnL.setup();
}

void loop() {
//	pm.loop();
	btnR.loop();
	btnL.loop();
	ui.loop();
}
