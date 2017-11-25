#pragma once

#include <Arduino.h>

class TrafficLight
{
public:
	TrafficLight(byte redPin, byte yellowPin, byte greenPin)
		: redPin(redPin), yellowPin(yellowPin), greenPin(greenPin) {
		pinMode(redPin, OUTPUT);
		pinMode(yellowPin, OUTPUT);
		pinMode(greenPin, OUTPUT);
	}

	void redOn() const { digitalWrite(redPin, LOW); }
	void redOff() const { digitalWrite(redPin, HIGH); }
	void yellowOn() const { digitalWrite(yellowPin, LOW); }
	void yellowOff() const { digitalWrite(yellowPin, HIGH); }
	void greenOn() const { digitalWrite(greenPin, LOW); }
	void greenOff() const { digitalWrite(greenPin, HIGH); }

	void fullOff() const { redOff(); yellowOff(); greenOff(); }

	void switchToRed() const { fullOff(); redOn(); }
	void switchToYellow() const { fullOff(); yellowOn(); }
	void switchToGreen() const { fullOff(); greenOn(); }

private:
	byte redPin, yellowPin, greenPin;
};

