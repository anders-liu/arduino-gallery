#include <Arduino.h>

// TX LED = PD5
// RX LED = PB0

bool flag = false;

void setup() {
	// Set TX/RX as output.
	DDRD |= (1 << DDD5);
	DDRB |= (1 << DDB0);
}

void loop() {
	if (flag)
	{
		PORTD &= ~(1 << PORTD5);
		PORTB |= (1 << PORTB0);
	}
	else {
		PORTD |= (1 << PORTD5);
		PORTB &= ~(1 << PORTB0);
	}

	delay(400);
	flag = !flag;
}
