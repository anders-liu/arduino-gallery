#pragma once

#include <Arduino.h>
#include "def.h"

enum EpdStatus {
	EPD_S_READY,
	EPD_S_BOOTING,
	EPD_S_SLEEPING,
	EPD_S_BUSY,
	EPD_S_TIMEOUT,
	EPD_S_ERROR,
};

#define EPD_INTERVAL_BOOTING     ((uint32_t)2000)
#define EPD_INTERVAL_TIMEOUT     ((uint32_t)2000)

#define EPD_CMD_UNKNOWN          ((uint8_t)0xFF)

#define EPD_CMD_HANDSHAKE        ((uint8_t)0x00)
#define EPD_CMD_GET_STORE        ((uint8_t)0x06)
#define EPD_CMD_SET_STORE        ((uint8_t)0x07)
#define EPD_CMD_SLEEP            ((uint8_t)0x08)
#define EPD_CMD_REFRESH          ((uint8_t)0x0A)

#define EPD_CMD_DRAW_POINT       ((uint8_t)0x20)
#define EPD_CMD_DRAW_LINE        ((uint8_t)0x22)
#define EPD_CMD_FILL_RECT        ((uint8_t)0x24)
#define EPD_CMD_DRAW_RECT        ((uint8_t)0x25)
#define EPD_CMD_DRAW_CIRCLE      ((uint8_t)0x26)
#define EPD_CMD_FILL_CIRCLE      ((uint8_t)0x27)
#define EPD_CMD_DRAW_TRIANGLE    ((uint8_t)0x28)
#define EPD_CMD_FILL_TRIANGLE    ((uint8_t)0x29)
#define EPD_CMD_CLEAR            ((uint8_t)0x2E)

#define EPD_CMD_DRAW_TEXT        ((uint8_t)0x30)
#define EPD_CMD_DRAW_IMAGE       ((uint8_t)0x70)

#define EPD_FILE_NAME_LEN        ((uint8_t)10)

class UartEpd {
public:
	UartEpd() {}

	void setup() {
		pinMode(PIN_EPD_RST, OUTPUT);
		pinMode(PIN_EPD_WKUP, OUTPUT);
		digitalWrite(PIN_EPD_RST, LOW);
		digitalWrite(PIN_EPD_WKUP, HIGH);
		lastMillis = millis();
	}

	void loop() {
		switch (status)
		{
		case EPD_S_BOOTING: {
			if (millis() - lastMillis > EPD_INTERVAL_BOOTING) {
				status = EPD_S_READY;
				lastMillis = millis();
			}
			break;
		}
		case EPD_S_BUSY: {
			if (Serial1.available()) {
				String s = Serial1.readStringUntil('\n');

#if THIS_APP_USE_SERIAL_DEBUG
				Serial.print("Uart1: ");
				Serial.println(s);
#endif

				switch (lastCommand) {
				case EPD_CMD_DRAW_IMAGE: {
					if (s[0] == 'F' || (s.length() > 2 && s[2] == 'F')) {
						// Echoing 'File:xxx', do nothing.
						// Sometimes 'OKFile:xxx'.
					}
					else if (s[0] == 'E') {
						// 'Error:xxx'
						status = EPD_S_ERROR;
					}
					else {
						status = EPD_S_READY;
					}
					break;
				}
				default: {
					status = EPD_S_READY;
					break;
				}
				}
			}

			if (status == EPD_S_BUSY
				&& (millis() - lastMillis > EPD_INTERVAL_TIMEOUT)) {
				status = EPD_S_TIMEOUT;
				lastMillis = millis();
			}
			break;
		}
		}
	}

	void startReset() {
		digitalWrite(PIN_EPD_RST, HIGH);
		digitalWrite(PIN_EPD_RST, LOW);
		status = EPD_S_BOOTING;
		lastMillis = millis();
	}

	void startWakeUp() {
		digitalWrite(PIN_EPD_WKUP, HIGH);
		digitalWrite(PIN_EPD_WKUP, LOW);
		status = EPD_S_BOOTING;
		lastMillis = millis();
	}

	void resetStatus() {
		switch (status) {
		case EPD_S_TIMEOUT:
		case EPD_S_ERROR:
			status = EPD_S_READY;
			break;
		}
	}

	EpdStatus getStatus() { return status; }

public:
	void startHandshake() {
		uint8_t cmd[] = { 0xA5, 0x00, 0x09, 0x00, 0xCC, 0x33, 0xC3, 0x3C, 0xAC };
		sendCommand(cmd, 0x09);
	}

	void startSetStorageSD() {
		uint8_t cmd[] = { 0xA5, 0x00, 0x0A, 0x07, 0x01, 0xCC, 0x33, 0xC3, 0x3C, 0xA9 };
		sendCommand(cmd, 0x0A);
	}

	void startClear() {
		uint8_t cmd[] = { 0xA5, 0x00, 0x09, 0x2E, 0xCC, 0x33, 0xC3, 0x3C, 0x82 };
		sendCommand(cmd, 0x09);
	}

	void startRefresh() {
		uint8_t cmd[] = { 0xA5, 0x00, 0x09, 0x0A, 0xCC, 0x33, 0xC3, 0x3C, 0xA6 };
		sendCommand(cmd, 0x09);
	}

	void startSleep() {
		uint8_t cmd[] = { 0xA5, 0x00, 0x09, 0x08, 0xCC, 0x33, 0xC3, 0x3C, 0xA4 };
		sendCommand(cmd, 0x09);
		status = EPD_S_SLEEPING;
	}

	void startDrawImage(uint16_t x, uint16_t y, const char *fn) {
		uint8_t cmd[24] = { 0xA5 };
		uint8_t fnlen = 0;
		while (fnlen <= EPD_FILE_NAME_LEN && fn[fnlen]) fnlen++;
		uint16_t len = fnlen + 1 + 0x0D;
		cmd[1] = (uint8_t)((len & 0xFF00) >> 8);
		cmd[2] = (uint8_t)(len & 0x00FF);
		cmd[3] = EPD_CMD_DRAW_IMAGE;
		cmd[4] = (uint8_t)((x & 0xFF00) >> 8);
		cmd[5] = (uint8_t)(x & 0x00FF);
		cmd[6] = (uint8_t)((y & 0xFF00) >> 8);
		cmd[7] = (uint8_t)(y & 0x00FF);
		for (uint8_t i = 0; i < fnlen; i++) {
			cmd[8 + i] = (uint8_t)fn[i];
		}
		cmd[8 + fnlen] = 0;
		cmd[8 + fnlen + 1] = 0xCC;
		cmd[8 + fnlen + 2] = 0x33;
		cmd[8 + fnlen + 3] = 0xC3;
		cmd[8 + fnlen + 4] = 0x3C;
		uint8_t parity = cmd[0];
		for (uint8_t i = 1; i < len - 1; i++) {
			parity ^= cmd[i];
		}
		cmd[len - 1] = parity;

		sendCommand(cmd, len);
	}

private:
	void sendCommand(uint8_t *buf, uint16_t len) {
#if THIS_APP_USE_SERIAL_DEBUG
		Serial.println("UartEpd.sendCommand");
		Serial.print("status: ");
		Serial.println(status);
#endif

		//if (status == EPD_S_BOOTING || status == EPD_S_BUSY || status == EPD_S_SLEEPING) {
		//	return;
		//}

#if THIS_APP_USE_SERIAL_DEBUG
		for (uint8_t i = 0; i < len; i++) {
			Serial.print("0x");
			Serial.print(buf[i], HEX);
			Serial.print(" ");
		}
		Serial.println();
#endif

		lastCommand = buf[3];
		status = EPD_S_BUSY;
		lastMillis = millis();
		Serial1.write(buf, len);
	}

private:
	EpdStatus status = EPD_S_READY;
	uint8_t /**/ lastCommand = EPD_CMD_UNKNOWN;
	uint32_t lastMillis;
};
