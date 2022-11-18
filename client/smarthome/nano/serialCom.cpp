#include "header.h"

#define PINGGER false
#define WAIT true

// Hardware serial
#define Rx 5
#define Tx 6
// esp
// #define Rx D3
// #define Tx D4
SoftwareSerial esp(Rx, Tx);
String serialMsg = "";
String pendingMsg;
bool fromSerial = false;
bool serialAvilable = true;
bool connectedToEsp = false;

// serial function

String basicRes(String type)
{
    return String(F("{  \"type\": \"")) + type + F("\"}");
}

bool breakeble()
{
    if (!pendingMsg.length())
        return false;
    bool hitNewLine = false;
    String pendingHolder = pendingMsg;
    pendingMsg = "";
    serialMsg = "";
    for (int i = 0; i <= pendingHolder.length(); i++)
    {
        if (pendingHolder[i] == '\0')
            continue;
        if (!hitNewLine && pendingHolder[i] == '\n')
        {
            hitNewLine = true;
            i++;
        }
        if (pendingHolder.length() - i <= 1 && (pendingHolder[i] == '\n' || pendingHolder[0] == '\0'))
            break;
        if (!hitNewLine)
            serialMsg += pendingHolder[i];
        else
            pendingMsg += pendingHolder[i];
    }
    return true;
}

bool readSerial()
{
    while (Serial.available())
    {
        pendingMsg += Serial.readString();
        fromSerial = true;
    }
    while (esp.available())
    {
        pendingMsg += esp.readString();
        fromSerial = false;
    }
    bool msgPending = breakeble();
    if (msgPending)
        Serial.println(String(F("Incoming from ")) + (fromSerial ? F("Serial") : F("esp")) + F(" :{\n ") + serialMsg + F("\n}\n "));
    if (pendingMsg == "\0")
        pendingMsg = "";
    return msgPending;
}

// private debug
void debugSend(String &msg, bool fromSerial = false)
{
    Serial.println(fromSerial ? (String(F("message: ")) + msg) : (String(F("sending to esp:{\n")) + msg + F("\n}")));
}

void stream(String msg, int size)
{
    Serial.println(F("Streaming to esp..."));
    esp.println(F("S"));
    esp.println(size);
    esp.println(msg);
    debugSend(msg);
}

void sendCom(String msg, int size, bool toSerial = false)
{
    Serial.println(String(F("pending message with size of ")) + String(size) + F(" bytes"));
    if (!toSerial)
        esp.println(size);
    while (!readSerial() && serialMsg[0] != '!')
        ;
    Serial.print("response is:");
    Serial.println(serialMsg);
    if (!toSerial)
        esp.println(msg);
    debugSend(msg, size);
}

void SerialInit()
{
    Serial.println(F("hello from nano"));
    do
    {
        Serial.println(F("connecting to esp..."));
        esp.begin(19200);
        delay(1000);
    } while (!esp);
    Serial.println(F("\nesp connected\n"));
#if PINGGER
    sendCom(basicRes(F("ping")), 48);
    connectedToEsp = true;
#endif
#if WAIT
    while (!connectedToEsp)
        SerialRun();
#endif
}

void SerialRun()
{
    /* Serial Comunication*/ {
        if (readSerial())
        {
            if (!fromSerial && !serialAvilable)
                return;
            bool isStream = serialMsg == F("stream");
            if (isStream)
            {
                Serial.println(F("stream receaved"));
                if (!readSerial())
                {
                    Serial.println(F("Missing size!"));
                    return;
                }
            }
            unsigned int size = serialMsg.toInt();
            // if its not a valid number
            if (size <= 0)
            {
                Serial.println(String(F("invalid size, asumed as miscom.\nreceaved data is: ")) + serialMsg);
                return;
            }
            Serial.println(String(F("incoming msg with size of ")) + String(size) + F(" bytes"));
            if (isStream)
            {
                if (!readSerial())
                {
                    Serial.println(F("Missing object!"));
                    return;
                }
            }
            else
            {
                // if its from serial
                if (fromSerial)
                    Serial.println(F("!"));
                else
                    esp.println(F("!"));
                // wait for the data
                while (!readSerial())
                    ;
            }
            DynamicJsonDocument doc(size);
            DeserializationError err = deserializeJson(doc, serialMsg);
            if (err)
            {
                String errorMsg = err.f_str();
                Serial.println(String(F("!!!!!!!!!!!!!\nError: ")) + errorMsg + F("\n!!!!!!!!!!!!!"));
                return;
            }
            if (doc[F("type")] == F("ping"))
            {
                serialAvilable = true;
                Serial.println(F("serial com activated from ping"));
#if !PINGGER
                connectedToEsp = true;
#endif
                return;
            }
            if (doc[F("type")] == F("disconnect!"))
            {
                sendCom(basicRes("disconnectForward"), 192);
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