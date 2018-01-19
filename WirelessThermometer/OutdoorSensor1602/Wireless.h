#pragma once

#include <Arduino.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "WirelessMessage.h"
#include "DataStore.h"

class Wireless {
public:
	Wireless(byte cePin, byte csnPin, byte channel, byte *txAddr, byte *rxAddr)
		: radio(cePin, csnPin), channel(channel), txAddr(txAddr), rxAddr(rxAddr) {
	}

public:
	void setup() {
		radio.begin();
		radio.enableDynamicPayloads();
		radio.setCRCLength(RF24_CRC_16);
		radio.setPALevel(RF24_PA_HIGH);
		radio.setAddressWidth(5);
		radio.setChannel(channel);
		radio.openReadingPipe(1, rxAddr);
		radio.openWritingPipe(txAddr);
		radio.startListening();
	}

	void loop() {
		loopSendThermometerData();
		loopReceive();
		loopTimeoutCheck();
	}

private:
	void loopSendThermometerData();
	void loopReceive();
	void loopTimeoutCheck();

private:
	RF24 radio;
	byte channel;
	byte *txAddr;
	byte *rxAddr;
	uint32_t lastTimeoutCheckMillis = 0;
};