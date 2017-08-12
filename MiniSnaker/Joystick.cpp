#include "Joystick.h"

#define KEY_CHECK_INTERVAL 5
#define POS_SH 800
#define NEG_SH 200

void Joystick::setup()
{
    pinMode(swPin, INPUT_PULLUP);
}

void Joystick::loop()
{
    unsigned long ms = millis();
    if (ms - lastCheckTime > KEY_CHECK_INTERVAL)
    {
        xBuf[bufPtr] = analogRead(xPin);
        yBuf[bufPtr] = analogRead(yPin);
        swBuf[bufPtr] = digitalRead(swPin);

        bufPtr++;
        if (bufPtr >= 4)
        {
            bufPtr = 0;
        }

        switch (bufPtr)
        {
        case 0:
            checkAnalogAndFire(
                xBuf[0], xBuf[1], xBuf[2], xBuf[3],
                xPosKeyDown, xNegKeyDown);
            checkAnalogAndFire(
                yBuf[0], yBuf[1], yBuf[2], yBuf[3],
                yPosKeyDown, yNegKeyDown);
            checkDigitalAndFire(
                swBuf[0], swBuf[1], swBuf[2], swBuf[3],
                swKeyDown);
            break;
        case 1:
            checkAnalogAndFire(xBuf[1], xBuf[2], xBuf[3], xBuf[0],
                               xPosKeyDown, xNegKeyDown);
            checkAnalogAndFire(
                yBuf[1], yBuf[2], yBuf[3], yBuf[0],
                yPosKeyDown, yNegKeyDown);
            checkDigitalAndFire(
                swBuf[1], swBuf[2], swBuf[3], swBuf[0],
                swKeyDown);
            break;
        case 2:
            checkAnalogAndFire(
                xBuf[2], xBuf[3], xBuf[0], xBuf[1],
                xPosKeyDown, xNegKeyDown);
            checkAnalogAndFire(
                yBuf[2], yBuf[3], yBuf[0], yBuf[1],
                yPosKeyDown, yNegKeyDown);
            checkDigitalAndFire(
                swBuf[2], swBuf[3], swBuf[0], swBuf[1],
                swKeyDown);
            break;
        case 3:
            checkAnalogAndFire(
                xBuf[3], xBuf[0], xBuf[1], xBuf[2],
                xPosKeyDown, xNegKeyDown);
            checkAnalogAndFire(
                yBuf[3], yBuf[0], yBuf[1], yBuf[2],
                yPosKeyDown, yNegKeyDown);
            checkDigitalAndFire(
                swBuf[3], swBuf[0], swBuf[1], swBuf[2],
                swKeyDown);
            break;
        }

        lastCheckTime = ms;
    }
}

void Joystick::checkAnalogAndFire(
    int v1, int v2, int v3, int v4,
    KeyEventHandler posKeyDown, KeyEventHandler negKeyDown)
{
    if (posKeyDown != nullptr &&
        v1 < POS_SH && v2 < POS_SH && v3 > POS_SH && v4 > POS_SH)
    {
        posKeyDown();
    }

    if (negKeyDown != nullptr &&
        v1 > NEG_SH && v2 > NEG_SH && v3 < NEG_SH && v4 < NEG_SH)
    {
        negKeyDown();
    }
}

void Joystick::checkDigitalAndFire(
    byte v1, byte v2, byte v3, byte v4,
    KeyEventHandler keyDown)
{
    if (keyDown != nullptr &&
        v1 == HIGH && v2 == HIGH && v3 == LOW && v4 == LOW)
    {
        keyDown();
    }
}