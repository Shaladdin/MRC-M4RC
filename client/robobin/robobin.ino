#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
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
bool pending = false;

// sensor rojak
#define metalTrig D5
#define metalEch D6
#define nonMetalTrig D8
#define nonMetalEch D7

class sensorJarak
{
    // pins
    int triger, echo;

public:
    // reading from last reading
    float distanse;
    // get distance
    float getDistance()
    {
        digitalWrite(triger, HIGH);
        delayMicroseconds(10);
        digitalWrite(triger, LOW);
        int echoTime = pulseIn(echo, HIGH);
        return (echoTime / 2) / 29.0;
    }
    sensorJarak(int triger, int echo)
    {
        this->triger = triger;
        this->echo = echo;
        pinMode(triger, OUTPUT);
        pinMode(echo, INPUT);
        digitalWrite(triger, LOW);
    }
};

sensorJarak sampahMetal(metalTrig, metalEch);
sensorJarak sampahLain(nonMetalTrig, nonMetalEch);

// leds
#define LEDm D4
#define LEDn D3

// io
const int outputs[] = {metalTrig, nonMetalTrig, LEDm, LEDn};
const int inputs[] = {metalEch, nonMetalEch};

// setting
#define readingLength 100
#define penuh 10.0
#define metalEmpty 22.0
#define nonMetalEmpty 23.5

void setup()
{
    Serial.begin(115200);
    Serial.println(F("starting up!"));

    // io init
    for (int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++)
        pinMode(outputs[i], OUTPUT);
    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
        pinMode(inputs[i], INPUT);

    digitalWrite(LEDm, HIGH);
    digitalWrite(LEDn, HIGH);

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
                        String msg = String(message.data());
                        Serial.println(F("Got Message: ") + msg);
                        String msgType, details, error;
                        {
                            DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);
                            DeserializationError err = deserializeJson(doc, msg);
                            if (err)
                            {
                                Serial.print(F("deserializeJson() failed with code "));
                                Serial.println(err.f_str());
                                return;
                            }
                            // collect data
                            msgType = doc[F("msg")].as<String>();
                            details = doc[F("details")].as<String>();
                            error = doc[F("err")].as<String>();
                        }
                        //  if identify request
                        if (msgType == F("identity requested"))
                        {
                            // self identify to server
                            String identifyMsg;
                            StaticJsonDocument<64> identifyDoc;
                            identifyDoc[F("msg")] = F("identity");
                            identifyDoc[F("device")] = F("roboBin");
                            serializeJson(identifyDoc, identifyMsg);
                            sendWs(identifyMsg);
                            return;
                        }
                        if (msgType == F("connected"))
                            activated = true; });
    digitalWrite(LEDm, LOW);
    digitalWrite(LEDn, LOW);
}

void loop()
{
    // check websocket
    if (ws.available())
        ws.poll();

    // code stop sign
    if (!activated)
        return;
    // code beloww only for when device is connected

    // read distances
    Serial.println("data:");
    float metalDistances, nonMetalDistances;
    for (int i = 0; i < readingLength; i++)
    {
        float met = sampahMetal.getDistance();
        float nonmet = sampahLain.getDistance();
        Serial.println();
        Serial.println(met);
        Serial.println(nonmet);
        metalDistances += met;
        nonMetalDistances += nonmet;
    }
    Serial.println("total: ");
    Serial.println(metalDistances);
    Serial.println(nonMetalDistances);
    metalDistances /= readingLength;
    nonMetalDistances /= readingLength;
    Serial.println("average:");
    Serial.println(metalDistances);
    Serial.println(nonMetalDistances);

    // Stream it to server
    String streamStr;
    StaticJsonDocument<192> doc;
    doc[F("type")] = F("stream");
    doc[F("stream")] = F("sensors");

    JsonObject data = doc.createNestedObject(F("data"));
    data[F("metal")] = metalDistances;
    data[F("nonMetal")] = nonMetalDistances;
    serializeJson(doc, streamStr);
    sendWs(streamStr);
}

// Server function
void sendWs(String &in)
{
    Serial.println(F("Sending: ") + in);
    ws.send(in);
}
