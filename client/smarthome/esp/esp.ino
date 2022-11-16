// connect all file
#include "header.h"

void setup()
{
    Serial.begin(115200);
    SerialInit();
    pinInit();
}

void loop()
{
    SerialRun();
}