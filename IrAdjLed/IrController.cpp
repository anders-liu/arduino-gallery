#include "IrController.h"

#define KEY_UP_THRESHOLD 200
#define DEBOUNCE_COUNT 1

IrKey getKey(decode_type_t type, unsigned long value) {
	switch (type) {
	case SAMSUNG:
		switch (value) {
		case 0xE0E040BF:
			return IRKEY_POWER;
		case 0xE0E0E01F:  // Vol+
		case 0xE0E048B7:  // CH+
		case 0xE0E006F9:  // Up
		case 0xE0E046B9:  // Right
			return IRKEY_PLUS;
		case 0xE0E0D02F:  // Vol-
		case 0xE0E008F7:  // CH-
		case 0xE0E08679:  // Down
		case 0xE0E0A659:  // Right
			return IRKEY_MINUS;
		}
		break;

	case XIAOMI1:
		switch (value) {
		case 0x0003CCCF:
			return IRKEY_POWER;
		case 0x0008605B:  // Up
		case 0x000860C2:  // Right
		case 0x000860E0:  // +
			return IRKEY_PLUS;
		case 0x00086068:  // Down
		case 0x000860B5:  // Left
		case 0x000860F1:  // -
			return IRKEY_MINUS;
		}
		break;
	}

	return IRKEY_UNKNOWN;
}

void IrController::loop() {
	decode_results result;

	if (millis() - lastMillis > KEY_UP_THRESHOLD) {
		if (lastFiredKey != IRKEY_UNKNOWN) {
			onKeyUp(lastFiredKey);
		}
	}

	if (irRecv.decode(&result)) {
		if (result.decode_type == lastCodeType && result.value == lastCodeValue) {
			debounceCount++;
			if (debounceCount >= DEBOUNCE_COUNT) {
				debounceCount = 0;

				IrKey key = getKey(lastCodeType, lastCodeValue);
				if (key != IRKEY_UNKNOWN) {
					onKeyDown(key);
				}
			}
		}
		else {
			lastCodeType = result.decode_type;
			lastCodeValue = result.value;
		}

		lastMillis = millis();
		irRecv.resume();
	}
}
