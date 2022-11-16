#include "header.h"

// Hardware serial
#define Rx 5
#define Tx 6
// esp
// #define Rx D3
// #define Tx D4
SoftwareSerial esp(Rx, Tx);
String serialMsg = "";
bool fromSerial = false;
bool serialAvilable = true;

// serial function

String basicRes(String type)
{
    return String(F("{  \"type\": \"")) + type + F("\"}");
}
bool readSerial()
{
    bool msgPending = false;
    while (Serial.available())
    {
        serialMsg = Serial.readString();
        msgPending = true;
        fromSerial = true;
    }
    while (esp.available())
    {
        serialMsg = esp.readString();
        msgPending = true;
        fromSerial = false;
    }
    if (msgPending)
        Serial.println(String(F("Incoming from ")) + (fromSerial ? F("Serial") : F("esp")) + F(" :{\n ") + serialMsg + F("\n}\n "));
    return msgPending;
}
void Send(String msg, int size, bool toSerial = false)
{
    Serial.println(String(F("pending message with size of ")) + String(size) + F(" bytes"));
    if (!toSerial)
        esp.println(size);
    while (!readSerial())
        ;
    if (!toSerial)
        esp.println(msg);
    Serial.println(toSerial ? String(F("message: ")) + msg : String(F("sending to nano:{\n")) + msg + F("\n}"));
}
void sendError(String msg, int size, bool toSerial = false)
{
    DynamicJsonDocument error(size);
    error[F("type")] = F("error");
    error[F("err")] = msg;
    String out;
    serializeJson(error, out);
    Send(out, size, toSerial);
}

void SerialInit()
{
    Serial.begin(115200);
    Serial.println(F("hello from nano"));
    do
    {
        Serial.println(F("connecting to esp..."));
        esp.begin(9600);
        delay(1000);
    } while (!esp);
    Serial.println(F("\nesp connected\n"));
}

void SerialRun()
{
    /* Serial Comunication*/ {
        if (readSerial())
        {
            if (!fromSerial && !serialAvilable)
                return;
            unsigned int size = serialMsg.toInt();
            // if its not a valid number
            if (size <= 0)
            {
                Serial.println(String(F("invalid size, asumed as miscom.\nreceaved data is: ")) + serialMsg);
                return;
            }
            Serial.println(String(F("incoming msg with size of ")) + String(size) + F(" bytes"));

            DynamicJsonDocument doc(size);
            // if its from serial
            if (fromSerial)
                Serial.println(F("!"));
            else
                esp.println(F("!"));
            // wait for the data
            while (!readSerial())
                ;
            DeserializationError err = deserializeJson(doc, serialMsg);
            if (err)
            {
                String errorMsg = err.f_str();
                Serial.println(String(F("Error: ")) + errorMsg);
                sendError(errorMsg, 192, fromSerial);
                return;
            }
            if (doc[F("type")] == F("ping"))
            {
                Send(basicRes("pong"), 48, fromSerial);
                serialAvilable = true;
                Serial.println(F("serial com activated from ping"));
                return;
            }
            if (doc[F("type")] == F("pong"))
            {
                serialAvilable = true;
                Serial.println(F("serial com activated from pong"));
                return;
            }
            if (doc[F("type")] == F("disconnect!"))
            {
                Send(basicRes("disconnectForward"), 192);
                goto disconnectSerial;
            }
            if (doc[F("type")] == F("disconnectForward"))
                goto disconnectSerial;
        }
    }

    // simple "void"s
    return;
    // ↑ a stop sign for void loop

// disconnect other controller
disconnectSerial:
    Serial.println(F("disconnecting..."));
    serialAvilable = false;
    return;
}