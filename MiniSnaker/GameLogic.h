#ifndef GameLogic_h
#define GameLogic_h

#include <Arduino.h>

typedef void (*UpdatedEventHandler)(byte *buffer);

struct Point
{
    byte x : 4;
    byte y : 4;

    friend bool operator==(Point p1, Point p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }
};

class GameLogic
{
  public:
    GameLogic(byte *buffer, unsigned int interval)
        : buffer(buffer), interval(interval)
    {
    }

    void setup();
    void loop();

    void turnUp();
    void turnDown();
    void turnLeft();
    void turnRight();
    void click();

    void onUpdated(UpdatedEventHandler handler) { this->updated = handler; }

  private:
    byte *buffer;
    unsigned int interval;
    byte state;
    byte dir, newDir;
    bool on;

    Point body[64];
    Point food;
    byte bodyBegin, bodyEnd;
    unsigned long lastMillis;

    void initGame();
    void initForStart();
    void loopStart();
    void initForRuning();
    void loopRuning();
    void initForPaused();
    void loopPaused();
    void initForOver();
    void loopOver();
    void switchState(byte newState);
    void newFood();
    bool bodyContains(Point pt);
    void clear();
    void drawBody();
    void drawFood();
    void set(byte x, byte y, bool on);

    void fireUpdated()
    {
        if (updated)
            updated(buffer);
    }

    UpdatedEventHandler updated;
};

#endif