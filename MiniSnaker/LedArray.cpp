#include "LedArray.h"

#define OP_D0 1
#define OP_D1 2
#define OP_D2 3
#define OP_D3 4
#define OP_D4 5
#define OP_D5 6
#define OP_D6 7
#define OP_D7 8
#define OP_DECODE_MODE 0x09
#define OP_INTENSITY 0x0A
#define OP_SCAN_LIMIT 0x0B
#define OP_SHUTDOWN 0x0C
#define OP_DISPLAY_TEST 0x0F

void LedArray::setup()
{
    pinMode(dataPin, OUTPUT);
    pinMode(csPin, OUTPUT);
    pinMode(clkPin, OUTPUT);
    digitalWrite(csPin, HIGH);

    send(OP_DISPLAY_TEST, 0);
    send(OP_SCAN_LIMIT, 7);
    send(OP_DECODE_MODE, 0);
    send(OP_INTENSITY, 6);
    send(OP_SHUTDOWN, 1);

    for (byte i = OP_D0; i <= OP_D7; i++)
    {
        send(i, 0);
    }
}

void LedArray::write(byte *buffer)
{
    for (byte i = 0; i < 8; i++)
        send(i + 1, buffer[i]);
}

void LedArray::send(byte opCode, byte data)
{
    digitalWrite(csPin, LOW);
    shiftOut(dataPin, clkPin, MSBFIRST, opCode);
    shiftOut(dataPin, clkPin, MSBFIRST, data);
    digitalWrite(csPin, HIGH);
}