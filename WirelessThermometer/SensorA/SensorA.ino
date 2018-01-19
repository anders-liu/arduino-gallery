//
// Name:	SensorA.ino
// Created:	12/4/2017 10:03:02 PM
// Author:	Anders Liu
//
// Display: SSD1306 I2C.
//

#include "UI.h"
#include <stdio.h>
#include <Arduino.h>
#include <dht.h>
#include <Adafruit_SSD1306.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>

#define SERIAL_DEBUG 1

//
// Wiring
//

#define OLED_ADDR 0x3C

#define DHT22_PIN A2

#define NRF24_CSN_PIN 8
#define NRF24_CE_PIN 6

//
// Devices
//

dht DHT;
Adafruit_SSD1306 screen(OLED_ADDR);

//
// Debug Utilities
//

#if SERIAL_DEBUG
#define DBG_BEGIN() do { Serial.begin(115200); } while(0)
#define DBG_PRINT(...) Serial.print(__VA_ARGS__)
#define DBG_PRINTLN(...) Serial.println(__VA_ARGS__)

#define DBG_PRINT_DHT_DATA(t, h) do { \
	DBG_PRINT("SUCCESS: "); \
	DBG_PRINT(t); \
	DBG_PRINT("*C, "); \
	DBG_PRINT(h); \
	DBG_PRINTLN("%"); \
} while(0)

#define DBG_PRINT_DHT_ERROR(e) do { \
	DBG_PRINT("ERROR: "); \
	switch (e) { \
		case DHTLIB_ERROR_CHECKSUM: DBG_PRINTLN("DHTLIB_ERROR_CHECKSUM"); break; \
		case DHTLIB_ERROR_TIMEOUT: DBG_PRINTLN("DHTLIB_ERROR_TIMEOUT"); break; \
		case DHTLIB_ERROR_CONNECT: DBG_PRINTLN("DHTLIB_ERROR_CONNECT"); break; \
		case DHTLIB_ERROR_ACK_L: DBG_PRINTLN("DHTLIB_ERROR_ACK_L"); break; \
		case DHTLIB_ERROR_ACK_H: DBG_PRINTLN("DHTLIB_ERROR_ACK_H"); break; \
		default: DBG_PRINTLN("<UNKNOWN>"); break; \
	} \
} while (0)
#else
#define DBG_BEGIN()
#define DBG_PRINT(...)
#define DBG_PRINTLN(...)

#define DBG_PRINT_DHT_DATA(t, h)
#define DBG_PRINT_DHT_ERROR(e)
#endif // DEBUG

void setup() {
	DBG_BEGIN();
	DBG_PRINTLN("Begin");
}

void loop() {
	delay(2000);
	byte ret = DHT.read22(DHT22_PIN);
	if (ret == DHTLIB_OK) {
		signed char t = (signed char)DHT.temperature;
		unsigned char h = (unsigned char)DHT.humidity;

		char str[20];
		snprintf(str, 20, "%2dC, %3d%%", t, h);
		DBG_PRINTLN(str);
	}
	else {
		DBG_PRINT_DHT_ERROR(ret);
	}


	char str[20];
	sprintf(str, "|%3d|", 0);
	DBG_PRINTLN(str);
	sprintf(str, "|%3d|", 10);
	DBG_PRINTLN(str);
	sprintf(str, "|%3d|", -0);
	DBG_PRINTLN(str);
	sprintf(str, "|%3d|", -1);
	DBG_PRINTLN(str);
	sprintf(str, "|%3d|", -10);
	DBG_PRINTLN(str);
}
