#include "header.h"

RH_ASK driver;

void RopiInit()
{
    do
    {
        Serial.println(F("connecting to wireless"));
    } while (!driver.init());
    Serial.println(F("connected to wireless"));
}

void RopiRun()
{
    if (!(flame > 80.0))
        return;
    Serial.println(F("!!!!!!!!!!!!!!!!!!!\nAPI TERDETEKSI\n!!!!!!!!!!!!!!!!!!!"));
    const char *msg = "A";
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
}