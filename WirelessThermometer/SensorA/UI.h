// UI.h

#ifndef _UI_h
#define _UI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Adafruit_SSD1306.h>

class UI {
public:
	UI(const Adafruit_SSD1306 &screen) : screen(screen) {}
	void setup();

private:
	const Adafruit_SSD1306 &screen;
};

#endif

