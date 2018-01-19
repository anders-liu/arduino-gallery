/*
 Name:		Rf24Test.ino
 Created:	12/31/2017 11:26:09 AM
 Author:	ander
*/

#include <Arduino.h>
#include <RF24_config.h>
#include <RF24.h>
#include <nRF24L01.h>

#define DYNAMIC_PAYLOAD 1

#if 0
#define RF_CE  6
#define RF_CSN 8
#else
#define RF_CE  A0
#define RF_CSN A1
#endif

RF24 rf(RF_CE, RF_CSN);

enum Role { ROLE_TX, ROLE_RX };
Role role = ROLE_TX;

const uint8_t txAddr[] = { 0x3C, 0xDB, 0xDB, 0xEC, 0xCE };
const uint8_t rxAddr[] = { 0x3C, 0xCE, 0xEC, 0xDB, 0xBD };

const uint8_t *payload = (uint8_t *)"abcdefghijklmnopqrstuvwxyz";
#define MAX_PAYLOAD_SZ 26
#define FIXED_PAYLOAD_SZ 4

#define RF_CHANNEL 6

uint32_t lastSendMillis;
#define SEND_INTERVAL 1000

void setup() {
	Serial.begin(115200);
	randomSeed(analogRead(A3));

	rf.begin();
	rf.setChannel(RF_CHANNEL);
	rf.setPALevel(RF24_PA_LOW);
	rf.setAutoAck(1);
	rf.setRetries(15, 15);

#if DYNAMIC_PAYLOAD
	rf.enableDynamicPayloads();
#else
	rf.setPayloadSize(4);
#endif

	if (role == ROLE_TX) {
		rf.openWritingPipe(txAddr);
		rf.openReadingPipe(1, rxAddr);
	}
	else {
		rf.openWritingPipe(rxAddr);
		rf.openReadingPipe(1, txAddr);
		rf.startListening();
	}

	lastSendMillis = millis();
}

void loop() {
	if (role == ROLE_TX) {
		uint32_t ms = millis();
		if (ms - lastSendMillis > SEND_INTERVAL) {
			lastSendMillis = ms;

#if DYNAMIC_PAYLOAD
			uint8_t len = rand() % MAX_PAYLOAD_SZ + 1;
#else
			uint8_t len = FIXED_PAYLOAD_SZ;
#endif

			Serial.print("[TX] writing ");
#if DYNAMIC_PAYLOAD
			Serial.print("(dynamic) ");
#else
			Serial.print("(fixed) ");
#endif
			Serial.print(len);
			Serial.print(" chars... ");

			bool r = rf.write(payload, len);

			Serial.println(r ? "True" : "False");
		}
	}

	if (role == ROLE_RX) {
		uint8_t pipe;
		if (rf.available(&pipe)) {
			uint8_t buf[MAX_PAYLOAD_SZ + 1] = { 0 };

			uint8_t len = rf.getDynamicPayloadSize();
			rf.read(buf, len);

			Serial.print("[RX] pipe ");
			Serial.print(pipe);
			Serial.print(" received ");
			Serial.print(len);
			Serial.print(" chars: ");
			Serial.println((const char *)buf);
		}
	}

	// Changing role
	if (Serial.available()) {
		char c = toupper(Serial.read());
		if (c == 'T' && role == ROLE_RX) {
			Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));

			role = ROLE_TX;
			rf.stopListening();
			rf.openWritingPipe(txAddr);
			rf.openReadingPipe(1, rxAddr);
			rf.flush_rx();
			rf.flush_tx();
		}
		else if (c == 'R' && role == ROLE_TX) {
			Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));

			role = ROLE_RX;                // Become the primary receiver (pong back)
			rf.openWritingPipe(rxAddr);
			rf.openReadingPipe(1, txAddr);
			rf.flush_rx();
			rf.flush_tx();
			rf.startListening();
		}
	}
}
