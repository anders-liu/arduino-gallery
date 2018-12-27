// Wireless.h

#ifndef WIRELESS_H__
#define WIRELESS_H__

#include <arduino.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "WirelessMessage.h"

class Wireless {
public:
	Wireless(byte cePin, byte csnPin, byte channel, byte *txAddr, byte *rxAddr)
		: radio(cePin, csnPin), channel(channel), txAddr(txAddr), rxAddr(rxAddr) {
	}

public:
	void setup();
	void loop();

private:
	void loopSetHeartbeat();
	// Not actually used, but as a hartbeat signal to central controller.
	void loopSendThermometerData();
	void loopSendLightData();
	bool send(WLM *msg);
	void loopReceive();

private:
	RF24 radio;
	byte channel;
	byte *txAddr;
	byte *rxAddr;

	uint32_t lastThermometerSendMillis = 0;
	uint8_t retryThermometerSendCount = 0;

	uint32_t lastLightDataSendMillis = 0;
	uint32_t retryLightDataSendCount = 0;
};

#endif  // WIRELESS_H__

