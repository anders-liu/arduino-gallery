#include "GameLogic.h"

// 'Global' constants

// - Moving directions
#define DIR_U 0
#define DIR_D 1
#define DIR_L 2
#define DIR_R 3

// - Game status
#define GS_START 0
#define GS_RUNNING 1
#define GS_PAUSED 2
#define GS_OVER 3

void GameLogic::setup()
{
    lastMillis = 0;
    state = GS_START;
    initForStart();
}

void GameLogic::loop()
{
    if (state == GS_RUNNING)
        loopRuning();
    else if (state == GS_START)
        loopStart();
    else if (state == GS_PAUSED)
        loopPaused();
    else if (state == GS_OVER)
        loopOver();
}

void GameLogic::initGame()
{
    randomSeed(millis());
    clear();

    dir = newDir = DIR_R;
    body[0] = {1, 1};
    body[1] = {2, 1};
    for (byte i = 2; i < 64; i++)
        body[i] = {0, 0};
    bodyEnd = 0;
    bodyBegin = 1;

    newFood();
}

void GameLogic::initForStart()
{
    clear();
    food = {1, 1}; // Just reuse food variable.
    dir = DIR_R;
    on = true;
}

void GameLogic::loopStart()
{
    unsigned long ms = millis();
    if (ms - lastMillis > interval)
    {
        set(food.x, food.y, on);

        if (dir == DIR_R)
        {
            food.x++;
            if (food.x > 8)
            {
                food.x = 8;
                food.y++;
                dir = DIR_L;
            }
        }
        else if (dir == DIR_L)
        {
            food.x--;
            if (food.x < 1)
            {
                food.x = 1;
                food.y++;
                dir = DIR_R;

                if (food.y > 8)
                {
                    food.y = 1;
                    on = !on;
                }
            }
        }
        fireUpdated();
        lastMillis = ms;
    }
}

void GameLogic::initForRuning()
{
    drawBody();
    drawFood();
}

void GameLogic::loopRuning()
{
    unsigned long ms = millis();
    if (ms - lastMillis > interval)
    {
        // Update direction.
        if (newDir != dir)
            dir = newDir;

        // Calculate new head.
        Point np;
        if (dir == DIR_U)
            np = {body[bodyBegin].x, ((byte)(body[bodyBegin].y - 1))};
        else if (dir == DIR_D)
            np = {body[bodyBegin].x, ((byte)(body[bodyBegin].y + 1))};
        else if (dir == DIR_L)
            np = {((byte)(body[bodyBegin].x - 1)), body[bodyBegin].y};
        else if (dir == DIR_R)
            np = {((byte)(body[bodyBegin].x + 1)), body[bodyBegin].y};

        // Check game-over.
        if (np.x < 1 || np.x > 8 || np.y < 1 || np.y > 8 || bodyContains(np))
        {
            switchState(GS_OVER);
            return;
        }

        // Grow head.
        bodyBegin++;
        if (bodyBegin >= 64)
            bodyBegin = 0;

        body[bodyBegin] = np;
        set(np.x, np.y, true);

        // Grow tail and eat food if possible.
        if (np == food)
        {
            newFood();
            set(food.x, food.y, true);
        }
        else
        {
            set(body[bodyEnd].x, body[bodyEnd].y, false);
            bodyEnd++;
            if (bodyEnd >= 64)
                bodyEnd = 0;
        }

        fireUpdated();
        lastMillis = ms;
    }
}

void GameLogic::initForPaused()
{
    clear();
    on = true;
}

void GameLogic::loopPaused()
{
    unsigned long ms = millis();
    if (ms - lastMillis > interval)
    {
        if (on)
        {
            drawBody();
            drawFood();
        }
        else
        {
            clear();
        }

        on = !on;

        lastMillis = ms;
    }
}

void GameLogic::initForOver()
{
    on = true;
}

void GameLogic::loopOver()
{
    unsigned long ms = millis();
    if (ms - lastMillis > interval)
    {
        set(body[bodyBegin].x, body[bodyBegin].y, on);
        fireUpdated();
        on = !on;
        lastMillis = ms;
    }
}

void GameLogic::turnUp()
{
    if (state == GS_RUNNING && (dir == DIR_L || dir == DIR_R))
        newDir = DIR_U;
}

void GameLogic::turnDown()
{
    if (state == GS_RUNNING && (dir == DIR_L || dir == DIR_R))
        newDir = DIR_D;
}

void GameLogic::turnLeft()
{
    if (state == GS_RUNNING && (dir == DIR_U || dir == DIR_D))
        newDir = DIR_L;
}

void GameLogic::turnRight()
{
    if (state == GS_RUNNING && (dir == DIR_U || dir == DIR_D))
        newDir = DIR_R;
}

void GameLogic::click()
{
    if (state == GS_START)
        switchState(GS_PAUSED);
    else if (state == GS_RUNNING)
        switchState(GS_PAUSED);
    else if (state == GS_PAUSED)
        switchState(GS_RUNNING);
    else if (state == GS_OVER)
        switchState(GS_PAUSED);
}

void GameLogic::switchState(byte newState)
{
    if (state == GS_START && newState == GS_PAUSED)
    {
        initGame();
    }
    else if (state == GS_OVER && newState == GS_PAUSED)
    {
        initGame();
    }

    switch (newState)
    {
    case GS_START:
        initForStart();
        break;
    case GS_PAUSED:
        initForPaused();
        break;
    case GS_RUNNING:
        initForRuning();
        break;
    case GS_OVER:
        initForOver();
        break;
    }

    state = newState;
}

void GameLogic::newFood()
{
    Point nf;

    do
    {
        nf.x = random(8) + 1;
        nf.y = random(8) + 1;
    } while (bodyContains(nf));

    food = nf;
}

bool GameLogic::bodyContains(Point pt)
{
    if (bodyEnd <= bodyBegin)
    {
        for (byte i = bodyEnd; i <= bodyBegin; i++)
            if (body[i] == pt)
                return true;
    }
    else
    {
        for (byte i = 0; i <= bodyBegin; i++)
            if (body[i] == pt)
                return true;
        for (byte i = bodyEnd; i < 64; i++)
            if (body[i] == pt)
                return true;
    }

    return false;
}

void GameLogic::clear()
{
    for (byte i = 0; i < 8; i++)
        buffer[i] = 0;

    fireUpdated();
}

void GameLogic::drawBody()
{
    if (bodyEnd <= bodyBegin)
    {
        for (byte i = bodyEnd; i <= bodyBegin; i++)
            set(body[i].x, body[i].y, true);
    }
    else
    {
        for (byte i = 0; i <= bodyBegin; i++)
            set(body[i].x, body[i].y, true);
        for (byte i = bodyEnd; i < 64; i++)
            set(body[i].x, body[i].y, true);
    }

    fireUpdated();
}

void GameLogic::drawFood()
{
    set(food.x, food.y, true);
    fireUpdated();
}

void GameLogic::set(byte x, byte y, bool on)
{
    if (on)
        buffer[8 - x] |= (1 << (8 - y));
    else
        buffer[8 - x] &= ~(1 << (8 - y));
}