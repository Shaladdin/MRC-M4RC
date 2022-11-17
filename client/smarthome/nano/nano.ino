// connect all file
#include "header.h"

void setup()
{
    Serial.begin(115200);
    pinInit();
    SensorInit();
    SerialInit();
}

void loop()
{
    SerialRun();
    // wait to connect
    if (!connectedToEsp)
        return;
    SensorRun();
}