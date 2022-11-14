// connect all file
#include "header.h"

RH_ASK driver;

void setup()
{
    Serial.begin(115200);
    Serial.println(F("hello from nano"));
    SerialInit();
    do
        Serial.println(F("connecting to wireless"));
    while (!driver.init());
    Serial.println(F("connected to wireless"));
}

void loop()
{
    SerialRun();
}

void sendWireless(const char *msg)
{
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
}