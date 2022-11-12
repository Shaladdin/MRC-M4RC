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
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

// IOT var
const char *ssid = "Adzka";                             // Enter SSID
const char *password = "123456789";                     // Enter Password
const char *host = "Si-Santui-M4RC.shaladddin.repl.co"; // Enter server adress
const char *wsProtocol = "wss://";
const uint16_t port = 8080; // Enter server port
WebsocketsClient ws;
const char *deviceID = "smartHome"; // robotID (to identefied by server)

bool activated = false;
bool pending = false;

// outputs pin
#define buzzer D0
#define lamp D3
#define fan D4
const int outputs[] = {lamp, fan, buzzer};

// inputs pin
#define apiPin D1
// ir
#define irDalam D5
#define irLuar D6
const int inputs[] = {apiPin, irDalam, irLuar};

// sensors
#define mq2Pin A0
#define dhtPin D2
MQ2 sensorGas(mq2Pin);
DHT sensorSuhu(dhtPin, DHT11);

// sensors reading
float lpg, co, smoke,
    temp, humidity, hIndex;

// Hardware serial
#define Rx D4
#define Tx D3
SoftwareSerial nano(Rx, Tx);

// room detection
bool prevOut, preIn, netral = false;
int JumlahOrang = 0;

// security
bool securityMode = false;

void setup()
{
    Serial.begin(115200);
    nano.begin(4800);
    delay(1000);

    // initialize outputs pins
    for (int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++)
        pinMode(outputs[i], OUTPUT);
    // initialize inputs pins
    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
        pinMode(inputs[i], INPUT);

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
                        msgType = doc[F("msg")].as<String>();
                        details = doc[F("details")].as<String>();
                        error = doc[F("err")].as<String>();
                    }

                    Serial.println(F("type:") + msgType);
                    Serial.println(F("detail:") + details);
                    Serial.println(F("error:") + error);

                    if (msgType == F("identity requested"))
                    {
                        // self identify to server
                        String identifyMsg;
                        StaticJsonDocument<64> identifyDoc;
                        identifyDoc[F("msg")] = F("identity");
                        identifyDoc[F("device")] = F("smartHome");
                        serializeJson(identifyDoc, identifyMsg);
                        sendWs(identifyMsg);
                        return;
                    }
                    if(msgType == F("security mode")){
                        securityMode = details == F("1");
                        Serial.println(securityMode);
                    }
                    if (msgType == F("connected"))
                        activated = true; });
}

void loop()
{
    /*stream sensor*/ {
        // check websocket
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
                       F("\nhumid:\t") + String(humidity) +
                       F("\nhIndex:\t") + String(hIndex));

        // code stop sign
        if (!activated)
            return;
        // code beloww only for when device is connected

        // Stream it to server
        String streamStr;
        StaticJsonDocument<256> doc;

        doc[F("type")] = F("stream");
        doc[F("stream")] = F("sensors");

        JsonObject data = doc.createNestedObject(F("data"));
        data[F("suhu")] = temp;
        data[F("humidity")] = humidity;

        JsonObject data_gas = data.createNestedObject(F("gas"));
        data_gas[F("smoke")] = smoke;
        data_gas[F("co")] = co;
        data_gas[F("lpg")] = lpg;

        serializeJson(doc, streamStr);
        sendWs(streamStr);
    }

    /*security*/ {
        bool in = !digitalRead(irDalam) && !digitalRead(irLuar);
        Serial.println("logic: " + in ? "true" : "false");
        if (in && securityMode)
        {
            tone(buzzer, 1000);
            String notifyStr;
            StaticJsonDocument<64> doc;
            doc[F("type")] = F("notify");
            doc[F("notify")] = F("security");
            serializeJson(doc, notifyStr);
            sendWs(notifyStr);
        }
        if (!securityMode)
            noTone(buzzer);
    }

    /*mode otomatis*/ {

    }

    /*room detection*/ {
    }
}

// Server function
void sendWs(String &in)
{
    Serial.println(F("Sending: ") + in);
    ws.send(in);
}

// 