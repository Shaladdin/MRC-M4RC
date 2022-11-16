#include <Arduino.h>
#line 1 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\esp\\esp.ino"
#include "header.h"

#line 3 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\esp\\esp.ino"
void setup();
#line 9 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\esp\\esp.ino"
void loop();
#line 3 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\esp\\esp.ino"
void setup()
{
    Serial.begin(115200);
    pinInit();
}

void loop()
{
    RoomDetectionRun();
}
