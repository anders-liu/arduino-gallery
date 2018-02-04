#pragma once

#include <Arduino.h>

using fa_input_t = uint8_t;
using fa_status_t = uint8_t;

// Inputs
#define FA_I_RD     ((fa_input_t)0x01)    // Right key down.
#define FA_I_RUS    ((fa_input_t)0x02)    // Short right key up.
#define FA_I_RUL    ((fa_input_t)0x03)    // Long right key up.
#define FA_I_LD     ((fa_input_t)0x04)    // Left key down.
#define FA_I_LUS    ((fa_input_t)0x05)    // Short left key up.
#define FA_I_LUL    ((fa_input_t)0x06)    // Long left key up.

// Status
#define FA_S_START    ((fa_status_t)0x00)
#define FA_S_ERROR    ((fa_status_t)0xFF)

#define FA_S_RD       ((fa_status_t)0x01)
#define FA_S_LD       ((fa_status_t)0x02)

#define FA_S_ACCEPT_MASK       ((fa_status_t)0x80)

#define FA_S_ACCEPT_NEXT_PG    (((fa_status_t)0x10) | FA_S_ACCEPT_MASK)
#define FA_S_ACCEPT_PREV_PG    (((fa_status_t)0x20) | FA_S_ACCEPT_MASK)
#define FA_S_ACCEPT_NEXT_SEC   (((fa_status_t)0x30) | FA_S_ACCEPT_MASK)
#define FA_S_ACCEPT_PREV_SEC   (((fa_status_t)0x40) | FA_S_ACCEPT_MASK)

class FA {
public:
	FA() {}

	void reset() {
		status = FA_S_START;
	}

	void input(fa_input_t input) {
#if THIS_APP_USE_SERIAL_DEBUG
		Serial.print("fa.input:");
		Serial.print(input);
		Serial.print("  ");
		Serial.print(status);
#endif

		switch (status) {
		case FA_S_START:
			switch (input) {
			case FA_I_RD: status = FA_S_RD; break;
			case FA_I_LD: status = FA_S_LD; break;
			}
			break;
		case FA_S_RD:
			switch (input) {
			case FA_I_RUS: status = FA_S_ACCEPT_NEXT_PG; break;
			case FA_I_RUL: status = FA_S_ACCEPT_NEXT_SEC; break;
			default: status = FA_S_ERROR; break;
			}
			break;
		case FA_S_LD:
			switch (input) {
			case FA_I_LUS: status = FA_S_ACCEPT_PREV_PG; break;
			case FA_I_LUL: status = FA_S_ACCEPT_PREV_SEC; break;
			default: status = FA_S_ERROR; break;
			}
			break;
		}

#if THIS_APP_USE_SERIAL_DEBUG
		Serial.print("->");
		Serial.println(status);
#endif
	}

	fa_status_t getStatus() { return status; }

private:
	fa_status_t status = FA_S_START;
};
