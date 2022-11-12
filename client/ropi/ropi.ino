#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char *ssid = "Adzka"; // Enter SSID
const char *password = "123456789";

#define relay 0
#define relay_ 2

void setup()
{
    Serial.begin(74880);
    // connect to wifi
    WiFi.begin(ssid, password);
    Serial.println(F("\nConnecting to ") + String(ssid));
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(F("."));
        delay(500);
    }
    pinMode(relay, OUTPUT);
    pinMode(relay_, OUTPUT);

    Serial.println(F("\nConnected to Wifi"));
}

bool on = false;
void loop()
{
    on = !on;
    Serial.println(on);
    digitalWrite(relay, !on);
    delay(500);
}