#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

#define Relay 3

void setup()
{
    Serial.begin(115200); // Debugging only
    if (!driver.init())
        Serial.println("init faiRelay");
    pinMode(Relay, OUTPUT);
    digitalWrite(Relay, HIGH);
}

void loop()
{
    uint8_t buf[12];
    uint8_t buflen = sizeof(buf);
    bool incoming = driver.recv(buf, &buflen);
    Serial.println(incoming);
    if (incoming)
        digitalWrite(Relay, LOW);
}