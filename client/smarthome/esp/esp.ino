// connect all file
#include "header.h"

void setup()
{
    SerialInit();
    pinInit();
}

void loop()
{
    SerialRun();
}