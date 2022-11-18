#include "header.h"

#define OTOMATIS 0
#define MANUAL 1
bool homeMode;
float maxBright, maxFlame, maxTemp;

const int turnOffOnEmpty[] = {lamp, fan};
void OtomatisRun()
{
    if (emptyRoom)
    {
        for (int x : turnOffOnEmpty)
            digitalWrite(x, LOW);
        return;
    }
    // code bellow is only on when someone is on the room
    digitalWrite(lamp, light > maxBright);
    digitalWrite(fan, temp > maxTemp);
}

// runned on webSocket.cpp
void ManualRun(){

}

void SmartHome()
{

}