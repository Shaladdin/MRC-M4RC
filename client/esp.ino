/*
    Esp8266 Websockets Client
    This sketch:
        1. Connects to a WiFi network
        2. Connects to a Websockets server
        3. Sends the websockets server a message ("Hello Server")
        4. Prints all incoming messages while the connection is open
    Hardware:
        For this sketch you only need an ESP8266 board.
    Created 15/02/2019
    By Gil Maimon
    https://github.com/gilmaimon/ArduinoWebsockets
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
using namespace websockets;

const char *ssid = "Adzka";                       // Enter SSID
const char *password = "123456789";               // Enter Password
const char *host = "mrc-m4rc.shaladddin.repl.co"; // Enter server adress
const char *wsProtocol = "wss://";
const uint16_t port = 8080; // Enter server port

WebsocketsClient client;
void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.println(F("Connecting to ") + String(ssid));
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
    }

    Serial.println("Connected to Wifi, Connecting to server.");
    // try to connect to Websockets server
    bool connectedToServer = false;
    while (!connectedToServer)
    {
        connectedToServer = client.connect(wsProtocol + String(host), port, "/");
        Serial.println("Attemp to connect to Websockets server");
    }
    // run callback when messages are received
    client.onMessage([&](WebsocketsMessage message)
                     {
        Serial.print("Got Message: ");
        Serial.println(message.data()); });
}

void loop()
{
    // let the websockets client check for incoming messages
    if (client.available())
        client.poll();
    delay(500);
}