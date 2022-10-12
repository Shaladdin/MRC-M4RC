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
#include <MQ2.h>
#include <DHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
using namespace websockets;

// IOT var
const char *ssid = "Adzka";                       // Enter SSID
const char *password = "123456789";               // Enter Password
const char *host = "mrc-m4rc.shaladddin.repl.co"; // Enter server adress
const char *wsProtocol = "wss://";
const uint16_t port = 8080; // Enter server port
WebsocketsClient ws;
const char *deviceID = "smartHome"; // robotID (to identefied by server)

bool activated = false;
// bool pending = false;

// outputs pin
#define lamp D3
#define fan D2
#define indicator LED_BUILTIN
const int outputs[] = {lamp, fan, LED_BUILTIN};

// sensors
#define mq2Pin A0
#define dhtPin D4
MQ2 sensorGas(mq2Pin);
DHT sensorSuhu(dhtPin, DHT11);

// sensors reading
float lpg, co, smoke,
    temp, humidity, hIndex;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // initialize pins
    for (int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++)
        pinMode(outputs[i], OUTPUT);

    // initialize sensors
    sensorGas.begin();
    sensorSuhu.begin();

    // connect to wifi
    WiFi.begin(ssid, password);
    Serial.println(F("\nConnecting to ") + String(ssid));
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(F("."));
        delay(500);
    }

    Serial.println(F("\nConnected to Wifi, Connecting to server."));
    // try to connect to Websockets server
    bool connectedToServer = false;
    while (!connectedToServer)
    {
        connectedToServer = ws.connect(wsProtocol + String(host));
        Serial.println(F("Attemp to connect to Websockets server"));
    }
    Serial.println(F("connected to websockets"));
    // run callback when messages are received
    ws.onMessage([&](WebsocketsMessage message)
                 {
                     Serial.println(F("Got Message: ") + message.data());
                     char *type, details;
                     {
                        DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);
                        deserializeJson(doc, message);
                        type = doc[F("msg")];
                        details = doc[F("err")]; 
                     } });

    // self identify to server
    String identifyMsg;
    StaticJsonDocument<64> identifyDoc;
    identifyDoc[F("msg")] = F("identity");
    identifyDoc[F("device")] = F("smartHome");
    serializeJson(identifyDoc, identifyMsg);
    sendWs(identifyMsg);
}

void loop()
{
    if (ws.available())
        ws.poll();

    // Sensors read
    lpg = sensorGas.readLPG();
    co = sensorGas.readCO();
    smoke = sensorGas.readSmoke();
    temp = sensorSuhu.readTemperature();
    humidity = sensorSuhu.readHumidity();
    hIndex = sensorSuhu.computeHeatIndex(temp, humidity);

    // debugging stuff
    Serial.println(F("\nlpg:\t") + String(lpg) +
                   F("\nco:\t") + String(co) +
                   F("\nsmoke:\t") + String(smoke) +
                   F("\ntemp:\t") + String(temp) +
                   F("\nhumidity:\t") + String(humidity) +
                   F("\nhIndex:\t") + String(hIndex));

    // code stop sign
    if (!activated)
        return;
    // code beloww only for when device is connected

    // Stream it to server
    String streamStr;
    StaticJsonDocument<192> doc;
    doc[F("type")] = F("stream");
    doc[F("stream")] = F("sensors");

    JsonObject data = doc.createNestedObject(F("data"));
    data[F("suhu")] = temp;
    JsonObject data_gas = data.createNestedObject(F("gas"));
    data_gas[F("co")] = co;
    data_gas[F("lpg")] = lpg;
    data_gas[F("smoke")] = smoke;
    serializeJson(doc, streamStr);
    sendWs(streamStr);
}

// Server function
void sendWs(String &in)
{
    Serial.println(F("Sending: ") + in);
    ws.send(in);
}