#include "header.h"
bool securityMode = false;

void SecurityMode(String &msgType, String &details, String &error)
{
    if (msgType == F("security mode"))
    {
        securityMode = details == F("1");
        Serial.println(String(F("Security mode is ")) + (securityMode ? F("On") : F("Off")));
    }
}