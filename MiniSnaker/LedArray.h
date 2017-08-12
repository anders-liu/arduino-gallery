#ifndef LedArray_h
#define LedArray_h

#include <Arduino.h>

class LedArray
{
  public:
    LedArray(int dataPin, int csPin, int clkPin)
        : dataPin(dataPin), csPin(csPin), clkPin(clkPin)
    {
    }

    void setup();
    void write(byte *buffer);

  private:
    int dataPin, csPin, clkPin;
    void send(byte opCode, byte data);
};

#endif