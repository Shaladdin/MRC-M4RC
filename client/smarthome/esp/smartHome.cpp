#include "header.h"

#define OTOMATIS 0
#define MANUAL 1
bool controllMode;
float maxBright, maxFlame, maxTemp, maxGas;

const int turnOffOnEmpty[] = {lamp, fan};
void OtomatisRun()
{
    // if (emptyRoom)
    // {
    //     for (int x : turnOffOnEmpty)
    //         digitalWrite(x, LOW);
    //     return;
    // }
    // // code bellow is only on when someone is on the room
    // digitalWrite(lamp, light < maxBright);
    // digitalWrite(fan, temp > maxTemp);

    for (int x : turnOffOnEmpty)
        digitalWrite(x, !emptyRoom);
}

// runned on webSocket.cpp
void ManualRun()
{

}

void SmartHomeRun()
{
    if (controllMode == OTOMATIS)
        OtomatisRun();
}