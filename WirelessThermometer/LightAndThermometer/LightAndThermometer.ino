//
// Name:	LightAndThermometer.ino
// Created:	12/6/2017 11:03:19 PM
// Author:	Anders Liu
//
// Night light controlled by infrared remote, with DHT22 thermometer.
//
// Wires
//
// (Note, PWM pin can't be changed easily, it's associated with PWM timer output.)
// PIN10/OC1B    <--->  LED light PWM (NPN transistor drive)
//
// PIN0/RX/INT2  <--->  IR receiver
//
// PIN A2        <--->  DHT22
//
// I2C 0x3C      <--->  SSD1306
//
// PIN6          <--->  NRF24L01.CE
// PIN8          <--->  NRF24L01.CSN
// PIN15/SCK     <--->  NRF24L01.SCK
// PIN14/MISO    <--->  NRF24L01.MISO
// PIN16/MOSI    <--->  NRF24L01.MOSI
// PIN1/TX/INT3  <--->  NRF24L01.IRQ
//

#include <Arduino.h>
#include <dht.h>
#include <AL_LeonardoRxTxLed.h>

#include "UI.h"
#include "IrController.h"
#include "Sensor.h"
#include "Wireless.h"
#include "NightLight.h"

//
// Wires
//

#define PIN_PWM 10
#define PIN_IR 0
#define PIN_DHT A2
#define PIN_NRF_CE 6
#define PIN_NRF_CSN 8

#define OLED_ADDR 0x3C

//
// Constants
//

#define WL_CHANNEL 6

#define REMOTE_ID 1

// From central controller.
//static const byte rxPipeAddr[] = { 0xC3, 0xDB, 0xBD, 0xEC, 0xCE };  // For remmote A, Outdoor.
#if REMOTE_ID == 1
static const byte rxPipeAddr[] = { 0x66, 0xDB, 0xBD, 0xEC, 0xCE };  // For remmote B, room 1.
#elif REMOTE_ID == 2
static const byte rxPipeAddr[] = { 0x3C, 0xDB, 0xBD, 0xEC, 0xCE };  // For remmote C, room 2.
#elif REMOTE_ID == 3
static const byte rxPipeAddr[] = { 0x99, 0xDB, 0xBD, 0xEC, 0xCE };  // For remmote D, room 3.
#endif

// To one of central controller rx pipes.
//static const byte txPipeAddr[] = { 0xC3, 0xCE, 0xEC, 0xBD, 0xDB };  // For remmote A, Outdoor.
#if REMOTE_ID == 1
static const byte txPipeAddr[] = { 0x66, 0xCE, 0xEC, 0xBD, 0xDB };  // For remmote B, room 1.
#elif REMOTE_ID == 2
static const byte txPipeAddr[] = { 0x3C, 0xCE, 0xEC, 0xBD, 0xDB };  // For remmote C, room 2.
#elif REMOTE_ID == 3
static const byte txPipeAddr[] = { 0x99, 0xCE, 0xEC, 0xBD, 0xDB };  // For remmote D, room 3.
#endif

//
// Modules
//

AL_LeonardoRxTxLed rxtx;

UI ui(OLED_ADDR);
Sensor sensor(PIN_DHT);
IrController ir(PIN_IR);
NightLight light;
Wireless wireless(PIN_NRF_CE, PIN_NRF_CSN, WL_CHANNEL, txPipeAddr, rxPipeAddr);

//
// Constants & Variables
//

#define SENSOR_INTERVAL 4000
unsigned long lastSensorMillis = 0;

bool uiKeyHolding = false;
bool powerKeyHolding = false;

//
// Event Handlers
//

void onIrKeyDown(IrKey key) {
	switch (key)
	{
	case IRKEY_POWER:
		if (!powerKeyHolding) {
			powerKeyHolding = true;
			light.switchPower();
		}
		break;
	case IRKEY_PLUS:
		light.plus();
		break;
	case IRKEY_MINUS:
		light.minus();
		break;
	case IRKEY_UI:
		if (!uiKeyHolding) {
			uiKeyHolding = true;
			ui.switchUI();
		}
		break;
	}
}

void onIrKeyUp(IrKey key) {
	switch (key)
	{
	case IRKEY_POWER:
		powerKeyHolding = false;
		break;
	case IRKEY_UI:
		uiKeyHolding = false;
		break;
	}
}

//
// Enterpoint
//

void setup() {
	//Serial.begin(115200);

	ds.f.sensorStateReadyToShow = 1;
	ds.f.sensorValueReadyToShow = 1;

	rxtx.rxOff();
	rxtx.txOff();

	ui.setup();
	light.setup();
	sensor.setup();
	wireless.setup();

	ir.handleKeyDown(onIrKeyDown);
	ir.handleKeyUp(onIrKeyUp);
	ir.setup();
}

void loop() {
	sensor.loop();
	wireless.loop();
	ir.loop();
	light.loop();
	ui.loop();
}
