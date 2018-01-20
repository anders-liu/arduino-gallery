#pragma once

#include <Arduino.h>
#include "nRF24L01.h"
#include "RF24.h"

#include "WirelessMessage.h"
#include "DataStore.h"

#define WL_MAX_REMOTE 4
#define WL_ADDR_WIDTH 5

class Wireless {
public:
	Wireless(
		const byte pinCE, const byte pinCSN,
		const byte channel, const byte **rxPipeAddrs, const byte **txPipeAddrs)
		: rf(pinCE, pinCSN)
		, channel(channel), rxPipeAddrs(rxPipeAddrs), txPipeAddrs(txPipeAddrs) {
	}

	void setup();

	void loop() {
		loopReceive();
		loopNoSignal();
		loopSendRtc();
		loopSendLightControl();
	}

private:
	void loopReceive();
	void loopNoSignal();
	void loopSendRtc();
	void loopSendLightControl();

	bool send(uint8_t channel, WLM *pMsg);

private:
	RF24 rf;
	const byte channel;
	const byte **rxPipeAddrs;
	const byte **txPipeAddrs;
	uint32_t lastNoSignalCheckMillis = 0;
};
