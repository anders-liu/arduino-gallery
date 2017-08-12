#ifndef Joystick_h
#define Joystick_h

#include <Arduino.h>

typedef void (*KeyEventHandler)(void);

class Joystick
{
  public:
    Joystick(byte xPin, byte yPin, byte swPin)
        : xPin(xPin), yPin(yPin), swPin(swPin),
          bufPtr(0), lastCheckTime(0),
          xPosKeyDown(nullptr), xNegKeyDown(nullptr),
          yPosKeyDown(nullptr), yNegKeyDown(nullptr),
          swKeyDown(nullptr)
    {
        for (byte i = 0; i < 4; i++)
        {
            xBuf[i] = yBuf[i] = 512;
            swBuf[i] = HIGH;
        }
    }

    void setup();
    void loop();

    void onXPosKeyDown(KeyEventHandler handler) { this->xPosKeyDown = handler; }
    void onXNegKeyDown(KeyEventHandler handler) { this->xNegKeyDown = handler; }
    void onYPosKeyDown(KeyEventHandler handler) { this->yPosKeyDown = handler; }
    void onYNegKeyDown(KeyEventHandler handler) { this->yNegKeyDown = handler; }
    void onSwKeyDown(KeyEventHandler handler) { this->swKeyDown = handler; }

  private:
    byte xPin, yPin, swPin;
    int xBuf[4], yBuf[4];
    byte swBuf[4];
    byte bufPtr;
    unsigned long lastCheckTime;

    void checkAnalogAndFire(
        int v1, int v2, int v3, int v4,
        KeyEventHandler posKeyDown, KeyEventHandler negKeyDown);
    void checkDigitalAndFire(
        byte v1, byte v2, byte v3, byte v4,
        KeyEventHandler keyDown);

    KeyEventHandler xPosKeyDown;
    KeyEventHandler xNegKeyDown;
    KeyEventHandler yPosKeyDown;
    KeyEventHandler yNegKeyDown;
    KeyEventHandler swKeyDown;
};

#endif