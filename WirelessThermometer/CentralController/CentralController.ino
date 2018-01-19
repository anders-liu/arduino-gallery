//
// Name:	CentralController.ino
// Created:	12/13/2017 9:23:54 PM
// Author:	Anders Liu
//

#include <Arduino.h>
#include <Wire.h>
#include <AL_LeonardoRxTxLed.h>

#include "UI.h"
#include "IR.h"
#include "Thermometer.h"
#include "Wireless.h"
#include "RTC.h"

#define RUN_UI_DEMO 0

//
// Wires
//

#define PIN_LCD_RS 0
#define PIN_LCD_RW 4
#define PIN_LCD_E 5
#define PIN_LCD_D4 A3
#define PIN_LCD_D5 A2
#define PIN_LCD_D6 A1
#define PIN_LCD_D7 A0

#define PIN_LCD_LED 10

#define PIN_NRF_CE 6
#define PIN_NRF_CSN 8

#define PIN_DHT 9

#define PIN_IR_RECV 7

#define DS3231_ADDR 0x68
#define AT24C32_ADDR 0x57

//
// Constants
//

#define WL_CHANNEL 6

static const byte txPipeAddrs[WL_MAX_REMOTE][WL_ADDR_WIDTH] = {
	{ 0xC3, 0xDB, 0xBD, 0xEC, 0xCE },  // For remmote A, outdoor.
	{ 0x66, 0xDB, 0xBD, 0xEC, 0xCE },  // For remmote B, room 1.
	{ 0x3C, 0xDB, 0xBD, 0xEC, 0xCE },  // For remmote C, room 2.
	{ 0x99, 0xDB, 0xBD, 0xEC, 0xCE },  // For remmote D, room 3.
};

static const byte rxPipeAddrs[WL_MAX_REMOTE][WL_ADDR_WIDTH] = {
	{ 0xC3, 0xCE, 0xEC, 0xBD, 0xDB },  // For remmote A, outdoor.
	{ 0x66, 0xCE, 0xEC, 0xBD, 0xDB },  // For remmote B, room 1.
	{ 0x3C, 0xCE, 0xEC, 0xBD, 0xDB },  // For remmote C, room 2.
	{ 0x99, 0xCE, 0xEC, 0xBD, 0xDB },  // For remmote D, room 3.
};

//
// Modules
//

AL_LeonardoRxTxLed rxtx;

UI ui(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_E,
	PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7,
	PIN_LCD_LED);
IR ir(PIN_IR_RECV);
Thermometer th(PIN_DHT);

Wireless wl(
	PIN_NRF_CE, PIN_NRF_CSN,
	WL_CHANNEL, (const byte **)rxPipeAddrs, (const byte **)txPipeAddrs);

RTC rtc(DS3231_ADDR);

//
// Entrypoints
//

#if RUN_UI_DEMO
#include "UI_Demo.cpp"
#endif

void setup() {
	Serial.begin(9600);
	Wire.begin();

	rxtx.rxOff();
	rxtx.txOff();

	ui.setup();
	ir.setup();
	th.setup();
	wl.setup();
	rtc.setup();

#if RUN_UI_DEMO
	UI_Demo_Setup();
#endif
}

void loop() {
#if !RUN_UI_DEMO
	ir.loop();
	th.loop();
	wl.loop();
	rtc.loop();
#endif

	ui.loop();

#if RUN_UI_DEMO
	UI_Demo_Loop();
#endif
}
