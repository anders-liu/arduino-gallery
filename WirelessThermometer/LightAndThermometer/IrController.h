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
	IrKey lastFiredKey;

	void onKeyDown(IrKey key) { lastFiredKey = key; if (keyDownHandler) keyDownHandler(key); }
	void onKeyUp(IrKey key) { lastFiredKey = IRKEY_UNKNOWN; if (keyUpHandler) keyUpHandler(key); }
};
