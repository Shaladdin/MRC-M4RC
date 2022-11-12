#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver;

void setup()
{
    Serial.begin(9600);
    do
        Serial.println(F("connecting to wireless"));
    while (!driver.init());
}

void loop()
{

    // transmiter ropi
}

void sendWireless(const char *msg)
{
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
}