#include "header.h"

#define DEBUG true

// called in serialCom.cpp:SerialRun()
void StreamSensors()
{
#if DEBUG
    Serial.println(String(F("\nlpg:\t")) + String(lpg) +
                   F("\nco:\t") + String(co) +
                   F("\nsmoke:\t") + String(smoke) +
                   F("\ntemp:\t") + String(temp) +
                   F("\nhumid:\t") + String(humidity) +
                   F("\nhIndex:\t") + String(hIndex) +
                   F("\nflame:\t") + String(flame) +
                   F("\nlight:\t") + String(light));
#endif
    
}