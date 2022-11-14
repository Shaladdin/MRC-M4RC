#include "header.h"

void setup()
{
    Serial.begin(115200);
    pinInit();
    SensorInit();
}

void loop()
{
    SensorRun();
}