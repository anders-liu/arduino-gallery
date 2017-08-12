#include "LedArray.h"
#include "Joystick.h"
#include "GameLogic.h"

#define LED_DIN A5
#define LED_CS A6
#define LED_CLK A7

#define JS_X A0
#define JS_Y A1
#define JS_SW A2

LedArray led(LED_DIN, LED_CS, LED_CLK);
Joystick js(JS_X, JS_Y, JS_SW);
byte buffer[8] = {0};
GameLogic game(buffer, 250);

void setup()
{
  led.setup();

  js.setup();
  js.onXPosKeyDown(handleXPosKeyDown);
  js.onXNegKeyDown(handleXNegKeyDown);
  js.onYPosKeyDown(handleYPosKeyDown);
  js.onYNegKeyDown(handleYNegKeyDown);
  js.onSwKeyDown(handleSwKeyDown);

  game.setup();
  game.onUpdated(handleGameUpdated);
}

void loop()
{
  js.loop();
  game.loop();
}

void handleXPosKeyDown()
{
  game.turnDown();
}

void handleXNegKeyDown()
{
  game.turnUp();
}

void handleYPosKeyDown()
{
  game.turnLeft();
}

void handleYNegKeyDown()
{
  game.turnRight();
}

void handleSwKeyDown()
{
  game.click();
}

void handleGameUpdated(byte *buffer)
{
  led.write(buffer);
}