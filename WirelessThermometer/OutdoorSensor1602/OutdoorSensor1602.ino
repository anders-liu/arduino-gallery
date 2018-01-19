#include <Arduino.h>
#include <AL_LeonardoRxTxLed.h>
#include "DataStore.h"
#include "UI.h"
#include "Sensor.h"
#include "Wireless.h"
#include "WirelessMessage.h"

//
// Wires
//

#define PIN_LCD_RS 9
#define PIN_LCD_RW 8
#define PIN_LCD_E 7
#define PIN_LCD_D4 6
#define PIN_LCD_D5 5
#define PIN_LCD_D6 4
#define PIN_LCD_D7 3

#define PIN_LCD_LED 10

#define PIN_NRF_CE A0
#define PIN_NRF_CSN A1

#define PIN_DHT A2

//
// Constants
//

#define WL_CHANNEL 6

// From central controller.
static const byte rxPipeAddr[] = { 0xC3, 0xDB, 0xBD, 0xEC, 0xCE  };  // For remmote A, Outdoor.
//static const byte rxPipeAddr[] = { 0x66, 0xDB, 0xBD, 0xEC, 0xCE };  // For remmote B, room 1.
//static const byte rxPipeAddr[] = { 0x3C, 0xDB, 0xBD, 0xEC, 0xCE };  // For remmote C, room 2.
//static const byte rxPipeAddr[] = { 0x99, 0xDB, 0xBD, 0xEC, 0xCE };  // For remmote D, room 3.

// To one of central controller rx pipes.
static const byte txPipeAddr[] = { 0xC3, 0xCE, 0xEC, 0xBD, 0xDB  };  // For remmote A, Outdoor.
//static const byte txPipeAddr[] = { 0x66, 0xCE, 0xEC, 0xBD, 0xDB };  // For remmote B, room 1.
//static const byte txPipeAddr[] = { 0x3C, 0xCE, 0xEC, 0xBD, 0xDB };  // For remmote C, room 2.
//static const byte txPipeAddr[] = { 0x99, 0xCE, 0xEC, 0xBD, 0xDB };  // For remmote D, room 3.

//
// Modules
//

AL_LeonardoRxTxLed rxtx;

UI ui(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_E, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7, PIN_LCD_LED);
Sensor sensor(PIN_DHT);
Wireless wireless(PIN_NRF_CE, PIN_NRF_CSN, WL_CHANNEL, txPipeAddr, rxPipeAddr);

//
// Entrypoint
//

void setup() {
	ds.f.sensorStateReadyToShow = 1;
	ds.f.sensorValueReadyToShow = 1;

	Serial.begin(9600);

	rxtx.rxOff();
	rxtx.txOff();

	ui.setup();
	sensor.setup();
	wireless.setup();
}

void loop() {
	sensor.loop();
	wireless.loop();
	ui.loop();
}
