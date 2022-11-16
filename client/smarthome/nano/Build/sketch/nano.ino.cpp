#include <Arduino.h>
#line 1 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\nano\\nano.ino"
#include "header.h"

#line 3 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\nano\\nano.ino"
void setup();
#line 10 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\nano\\nano.ino"
void loop();
#line 3 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\nano\\nano.ino"
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
