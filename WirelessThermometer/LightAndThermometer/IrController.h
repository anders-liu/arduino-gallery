#pragma once

#include <IRremote.h>

enum IrKey {
	IRKEY_UNKNOWN,
	IRKEY_POWER,
	IRKEY_PLUS,
	IRKEY_MINUS,
	IRKEY_UI,
};

using IrKeyEventHandler = void(*)(IrKey);

class IrController
{
public:
	IrController(int recvpin)
		:irRecv(recvpin)
		, keyDownHandler(nullptr)
		, keyUpHandler(nullptr)
		, lastMillis(0)
		, debounceCount(0)
		, lastCodeType(UNKNOWN)
		, lastCodeValue(0)
		, lastFiredKey(IRKEY_UNKNOWN) {
	}

	void setup() {
		irRecv.enableIRIn();
	}

	void loop();

	void handleKeyDown(IrKeyEventHandler handler) { keyDownHandler = handler; }
	void handleKeyUp(IrKeyEventHandler handler) { keyUpHandler = handler; }

private:
	IRrecv irRecv;
	IrKeyEventHandler keyDownHandler;
	IrKeyEventHandler keyUpHandler;
	unsigned long lastMillis;
	byte debounceCount;
	decode_type_t lastCodeType;
	unsigned long lastCodeValue;
	IrKey lastFiredKey;

	void onKeyDown(IrKey key) { lastFiredKey = key; if (keyDownHandler) keyDownHandler(key); }
	void onKeyUp(IrKey key) { lastFiredKey = IRKEY_UNKNOWN; if (keyUpHandler) keyUpHandler(key); }
};
