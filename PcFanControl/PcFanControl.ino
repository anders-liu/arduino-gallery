//
// Name:    PcFanControl.ino
// Created: 10/30/2017 8:56:29 PM
// Author:  Anders Liu
//
// Control 4-wire PC fan, which uses 25kHz PWM to controll speed
// has a tachometer.
//
// This sketch is based on Pro Micro board (ATMega32u4) and uses its timer3.
//

#include <Arduino.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AL_PushButton.h>

//
// Wiring.
//
// Digital Pin 7 PE6 (INT6 / AIN0)             <---> Tachometer
// Digital Pin 10 (PB6/PCINT6/OC1B/OC4B/ADC13) <---> PWM (Speed Control)
//
// Digital Pin A0 <---> Button +
// Digital Pin A1 <---> Button -
//

#define PIN_TACHOMETER 7
#define PIN_PWM 10

#define PIN_PLUS A0
#define PIN_MINUS A1

#define OLED_ADDR 0x3C

AL_PushButton btnPlus(PIN_PLUS);
AL_PushButton btnMinus(PIN_MINUS);

Adafruit_SSD1306 oled;

#define MIN_SPEED_ID 1
#define MAX_SPEED_ID 11

const static uint8_t SPEED_TABLE[MAX_SPEED_ID] = {
	0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80
};

volatile uint8_t speedId = MIN_SPEED_ID;
volatile uint16_t count = 0;
volatile uint16_t countSecond = 0;
volatile bool secondUpdated = false;
volatile uint16_t timerExt = 0;

#define BUF_SZ 5
volatile uint8_t bufIdx = 0;
volatile uint8_t buf[BUF_SZ] = { 0 };

volatile uint8_t animCount = 0;
volatile bool animUpdated = false;
volatile uint8_t animId = 0;
#define ANIM_INTERVAL 2
#define ANIM_STEPS 8

#define SET_SPEED(id) { /*cli();*/ OCR1B = SPEED_TABLE[speedId - 1]; /*sei();*/ }

void setupTimer() {
	// Timer1
	OCR1B = SPEED_TABLE[speedId - 1];  // Default speed.
	ICR1 = 80;  // TOP = 80, PWM = 80*0.5us = 40us, 25kHz, PC fan standard.
	// COM1B1:COM1B0 = 11, output on OC1B, set on compare match, clear on TOP.
	bitSet(TCCR1A, COM1B1);
	bitSet(TCCR1A, COM1B0);
	// WGM13:0 = 1110, fast PWM, TOP=ICR1
	bitSet(TCCR1B, WGM13);
	bitSet(TCCR1B, WGM12);
	bitSet(TCCR1A, WGM11);
	bitClear(TCCR1A, WGM10);
	// CS2:0 = 010, Fclk/8, 2MHz, 0.5us
	bitClear(TCCR1B, CS12);
	bitSet(TCCR1B, CS11);
	bitClear(TCCR1B, CS10);
	// TOIE1 = 1, enable TOV1, for tachometer counting.
	bitSet(TIMSK1, TOIE1);
}

void setupTachoInterrupt() {
	// INT6
	// ISC61:ISC60 = 11, rising edge.
	bitSet(EICRB, ISC61);
	bitSet(EICRB, ISC60);
	// INT6 = 1, enable INT6.
	bitSet(EIMSK, INT6);
}

ISR(INT6_vect) {
	count++;

	animCount++;
	if (animCount == ANIM_INTERVAL) {
		animCount = 0;
		animUpdated = true;
	}
}

ISR(TIMER1_OVF_vect) {
	if (timerExt >= 25000) {
		timerExt = 0;
		secondUpdated = true;
		countSecond = count;

		buf[bufIdx] = countSecond;
		bufIdx = (bufIdx + 1) % BUF_SZ;

		count = 0;
	}
	else {
		timerExt++;
	}
}

void writeRpm(uint16_t rpm) {
	oled.fillRect(58, 16 - 1, 60, 16, BLACK);
	oled.setCursor(58, 16 - 1);
	char str[6] = { 0 };
	sprintf(str, "%5d", rpm);
	oled.print(str);
	oled.display();
}

void writeSpeed() {
	oled.fillRect(58, 32 + 1, 24, 16, BLACK);
	oled.setCursor(58, 32 + 1);
	char str[3] = { 0 };
	sprintf(str, "%2d", speedId - 1);
	oled.print(str);
	oled.display();
}

void updateAnim() {
	if (!animUpdated) {
		return;
	}

	animUpdated = false;

	// Clear
	oled.drawFastHLine(0, 0, 128, BLACK);
	oled.drawFastHLine(0, 1, 128, BLACK);
	oled.drawFastHLine(0, 62, 128, BLACK);
	oled.drawFastHLine(0, 63, 128, BLACK);
	oled.drawFastVLine(0, 2, 60, BLACK);
	oled.drawFastVLine(1, 2, 60, BLACK);
	oled.drawFastVLine(126, 2, 60, BLACK);
	oled.drawFastVLine(127, 2, 60, BLACK);

	// Draw
	uint8_t start = animId * 2;
	if (start >= 8) {
		oled.drawFastHLine(0, 0, (start - 7), WHITE);
		oled.drawFastHLine(0, 1, (start - 7), WHITE);

		oled.drawFastHLine(128 - (start - 7), 62, (start - 7), WHITE);
		oled.drawFastHLine(128 - (start - 7), 63, (start - 7), WHITE);

		oled.drawFastVLine(126, 0, (start - 7), WHITE);
		oled.drawFastVLine(127, 0, (start - 7), WHITE);

		oled.drawFastVLine(0, (start - 7), (start - 7), WHITE);
		oled.drawFastVLine(1, (start - 7), (start - 7), WHITE);
	}

	for (uint8_t x = start; x < 128; x += 16) {
		oled.drawFastHLine(x, 0, 8, WHITE);
		oled.drawFastHLine(x, 1, 8, WHITE);

		oled.drawFastHLine(128 - 8 - x, 62, 8, WHITE);
		oled.drawFastHLine(128 - 8 - x, 63, 8, WHITE);
	}

	for (uint8_t y = start; y < 64; y += 16) {
		oled.drawFastVLine(126, y, 8, WHITE);
		oled.drawFastVLine(127, y, 8, WHITE);

		oled.drawFastVLine(0, 64 - 8 - y, 8, WHITE);
		oled.drawFastVLine(1, 64 - 8 - y, 8, WHITE);
	}

	// Update
	oled.display();
	animId = (animId + 1) % ANIM_STEPS;
}

void onPlusKeyDown() {
	if (speedId < MAX_SPEED_ID) {
		speedId++;
		SET_SPEED(speedId);
		writeSpeed();
	}
}

void onMinusKeyDown() {
	if (speedId > MIN_SPEED_ID) {
		speedId--;
		SET_SPEED(speedId);
		writeSpeed();
	}
}

// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(PIN_TACHOMETER, INPUT_PULLUP);

	pinMode(PIN_PWM, OUTPUT);
	digitalWrite(PIN_PWM, HIGH);

	Serial.begin(115200);
	oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
	oled.clearDisplay();
	oled.display();

	//cli();
	setupTimer();
	setupTachoInterrupt();
	//sei();

	btnPlus.handleKeyDown(onPlusKeyDown);
	btnMinus.handleKeyDown(onMinusKeyDown);

	btnPlus.setup();
	btnMinus.setup();

	oled.setTextSize(2);
	oled.setTextColor(WHITE);

	oled.setCursor(10, 16 - 1);
	oled.print("RPM:    0");
	oled.setCursor(10, 32 + 1);
	oled.print("SPD:  /10");
	oled.display();

	writeSpeed();
}

// the loop function runs over and over again until power down or reset
void loop() {
	btnPlus.loop();
	btnMinus.loop();

	if (secondUpdated) {
		secondUpdated = false;

		uint16_t sum = 0;
		for (uint8_t i = 0; i < BUF_SZ; i++) {
			sum += buf[i];
		}
		uint16_t rpm = sum * 30 / BUF_SZ;

		writeRpm(rpm);
	}

	updateAnim();
}
