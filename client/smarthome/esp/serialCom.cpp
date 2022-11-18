#include "header.h"

#define PINGGER true
#define repingTimer 5000

// Hardware serial
#define Rx D3
#define Tx D4
// nano
// #define Rx 5
// #define Tx 6
SoftwareSerial nano(Rx, Tx);
String serialMsg = "";
bool fromSerial = false;
bool serialAvilable = true;
bool connectedToNano = false;

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
    while (nano.available())
    {
        serialMsg = nano.readString();
        msgPending = true;
        fromSerial = false;
    }
    if (msgPending)
        Serial.println(String(F("Incoming from ")) + (fromSerial ? F("Serial") : F("nano")) + F(" :{\n ") + serialMsg + F("\n}\n "));
    return msgPending;
}
void sendCom(String msg, int size, bool toSerial = false)
{
    Serial.println(String(F("pending message with size of ")) + String(size) + F(" bytes"));
    unsigned long int startStamp = millis();
    unsigned long int timer = 0;
    while (true)
    {
        if (timer > millis())
            if (readSerial() && serialMsg == "!")
                break;
            else
                continue;
        Serial.println(F("trying to connect..."));
        if (!toSerial)
            nano.println(size);
        timer = millis() + repingTimer;
    }
    if (!toSerial)
        nano.println(msg);
    Serial.println(toSerial ? String(F("message: ")) + msg : String(F("sending to nano:{\n")) + msg + F("\n}"));
    Serial.println(String(F("transfer time = ")) + String(millis() - startStamp) + F(" milisecond"));
}
void sendError(String msg, int size, bool toSerial = false)
{
    DynamicJsonDocument error(size);
    error[F("type")] = F("error");
    error[F("err")] = msg;
    String out;
    serializeJson(error, out);
    sendCom(out, size, toSerial);
}

// initialize serial
void SerialInit()
{
    Serial.println(F("hello from esp"));
    delay(1000);
    do
    {
        Serial.println(F("connecting to nano..."));
        nano.begin(9600);
        delay(1000);
        nano.println();
    } while (!nano);
    Serial.println(F("\nconnected to nano\n"));
#if PINGGER
    sendCom(basicRes(F("ping")), 48);
#endif
}

unsigned int MSGsize = 0;
unsigned long int pendingStamp;
bool waitingForMsg = false;

// run serial
void SerialRun()
{
    /* Serial Comunication*/ {
        if (readSerial())
        {
            if (!waitingForMsg)
            {
                pendingStamp = millis();
                if (!fromSerial && !serialAvilable)
                    return;
                MSGsize = serialMsg.toInt();
                // if its not a valid number
                if (MSGsize <= 0)
                {
                    Serial.println(String(F("invalid size, asumed as miscom.\nreceaved data is: ")) + serialMsg);
                    return;
                }
                Serial.println(String(F("incoming msg with size of ")) + String(MSGsize) + F(" bytes"));
                // if its from serial
                if (fromSerial)
                    Serial.println(F("!"));
                else
                    nano.println(F("!"));
                // wait for the data
                waitingForMsg = true;
                return;
            }
            Serial.println(String(F("pending time = ")) + String(millis() - pendingStamp) + F(" milisecond"));
            DynamicJsonDocument doc(MSGsize);
            DeserializationError err = deserializeJson(doc, serialMsg);
            waitingForMsg = false;
            if (err)
            {
                String errorMsg = err.f_str();
                Serial.println(String(F("Error: ") + errorMsg));
                sendError(errorMsg, 192, fromSerial);
                return;
            }
            if (doc[F("type")] == F("ping"))
            {
                sendCom(basicRes("pong"), 48, fromSerial);
                serialAvilable = true;
                Serial.println(F("serial com activated from ping"));
#if !PINGGER
                connectedToNano = true;
#endif
                return;
            }
            if (doc[F("type")] == F("pong"))
            {
                serialAvilable = true;
                Serial.println(F("serial com activated from pong"));
#if PINGGER
                connectedToNano = true;
#endif
                return;
            }
            if (doc[F("type")] == F("stream"))
            {
                if (doc[F("stream")] == F("sensors"))
                {
                    JsonObject data = doc[F("data")];
                    temp = data[F("suhu")];
                    humidity = data[F("humidity")];
                    // flame =
                    Serial.println(serialMsg);
                    Serial.println("msg D:<");
                    return;
                }
            }
            if (doc[F("type")] == F("disconnect!"))
            {
                sendCom(basicRes(F("disconnectForward")), 68);
                goto disconnectSerial;
            }
            if (doc[F("type")] == F("disconnectForward"))
                goto disconnectSerial;
        }

        // Sensors stream:
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
