#pragma once

#include <Arduino.h>
#include "UartEpd.h"
#include "FA.h"

#define UI_SHORT_KEYUP_INTERVAL ((uint32_t)1500)
#define UI_LONG_KEYUP_INTERVAL  ((uint32_t)4000)

using ui_input_t = uint8_t;

#define UI_I_RD ((ui_input_t)0x01)
#define UI_I_RU ((ui_input_t)0x02)
#define UI_I_LD ((ui_input_t)0x03)
#define UI_I_LU ((ui_input_t)0x04)

using ui_op_t = uint8_t;

#define OP_NEXT_PG  ((ui_op_t)1)
#define OP_PREV_PG  ((ui_op_t)2)
#define OP_NEXT_SEC ((ui_op_t)3)
#define OP_PREV_SEC ((ui_op_t)4)

class UI {
public:
	UI() { }

	void setup(uint16_t secID, uint16_t pgID) {
		epd.setup();
		delay(1200);
		epd.startSetStorageSD();
		delay(500);

		curSec = secID;
		curPg = pgID;
		display();
		delay(1500);
		epd.startRefresh();
	}

	void loop() {
		epd.loop();

		EpdStatus ds = epd.getStatus();

		if (ds == EPD_S_TIMEOUT) {
			// Ignore.
			epd.resetStatus();
		}

		if (ds == EPD_S_ERROR) {
			epd.resetStatus();

#if THIS_APP_USE_SERIAL_DEBUG
			Serial.print("ui.loop: EPD ERROR, lastOP=");
			Serial.println(lastOP);
#endif

			switch (lastOP) {
			case OP_NEXT_PG:
				lastOP = OP_NEXT_SEC;
				setNextSection();
				display();
				break;

			case OP_PREV_PG:
				lastOP = OP_PREV_SEC;
				setPrevSection();
				display();
				break;

			case OP_NEXT_SEC:
			case OP_PREV_SEC:
//				lastOP = 0;
				curSec = 0;
				curPg = 0;
				display();
				break;
			}
		}

		if (ds == EPD_S_READY) {
			switch (lastOP) {
			case OP_NEXT_PG:
			case OP_PREV_PG:
			case OP_NEXT_SEC:
			case OP_PREV_SEC:
				needRefresh = true;
				lastOP = 0;
			}

			if (needRefresh) {
				needRefresh = false;
				epd.startRefresh();
			}
		}
	}

	void input(ui_input_t input) {
#if THIS_APP_USE_SERIAL_DEBUG
		Serial.print("ui.input:");
		Serial.println(input);
#endif
		if (canAcceptInput()) {
			switch (input) {
			case UI_I_RD: fa.input(FA_I_RD); break;
			case UI_I_LD: fa.input(FA_I_LD); break;
			case UI_I_RU: checkLongKeyUp(FA_I_RUS, FA_I_RUL); break;
			case UI_I_LU: checkLongKeyUp(FA_I_LUS, FA_I_LUL); break;
			}

#if THIS_APP_USE_SERIAL_DEBUG
			Serial.print("ui.input:fa:");
			Serial.println(fa.getStatus());
#endif

			switch (fa.getStatus()) {
			case FA_S_ACCEPT_NEXT_PG: doOperation(OP_NEXT_PG); break;
			case FA_S_ACCEPT_PREV_PG: doOperation(OP_PREV_PG); break;
			case FA_S_ACCEPT_NEXT_SEC: doOperation(OP_NEXT_SEC); break;
			case FA_S_ACCEPT_PREV_SEC: doOperation(OP_PREV_SEC); break;
			}

			if (fa.getStatus() & FA_S_ACCEPT_MASK) {
				fa.reset();
			}

			lastInputMillis = millis();
		}
	}

private:
	bool canAcceptInput() {
		return epd.getStatus() == EPD_S_READY;
	}

	void checkLongKeyUp(fa_input_t shortInput, fa_input_t longInput) {
		uint32_t ms = millis();
		if (ms - lastInputMillis < UI_SHORT_KEYUP_INTERVAL) {
			fa.input(shortInput);
		}
		else if (ms - lastInputMillis < UI_LONG_KEYUP_INTERVAL) {
			fa.input(longInput);
		}
		else {
			fa.reset();
		}
	}

	void doOperation(ui_op_t op) {
		lastOP = op;
		switch (op) {
		case OP_NEXT_PG: setNextPage(); break;
		case OP_PREV_PG: setPrevPage(); break;
		case OP_NEXT_SEC: setNextSection(); break;
		case OP_PREV_SEC: setPrevSection(); break;
		}
		display();
	}

	void setNextPage() {
		if (curPg < 999) {
			curPg++;
		}
		else {
			setNextSection();
		}
	}

	void setPrevPage() {
		if (curPg > 0) {
			curPg--;
		}
		else {
			setPrevSection();
		}
	}

	void setNextSection() {
		curPg = 0;
		if (curSec < 99) {
			curSec++;
		}
		else {
			curSec = 0;
		}
	}

	void setPrevSection() {
		curPg = 0;
		if (curSec > 0) {
			curSec--;
		}
		else {
			curSec = 0;
		}
	}

	void display() {
		char fn[11] = { 0 };
		snprintf(fn, 11, "%02d_%03d.BMP", curSec, curPg);
		epd.startDrawImage(0, 0, fn);
	}

private:
	UartEpd epd;
	FA fa;
	uint32_t lastInputMillis = 0;
	ui_op_t lastOP = 0;
	bool needRefresh = false;

	uint16_t curSec = 0;
	uint16_t curPg = 0;
};
