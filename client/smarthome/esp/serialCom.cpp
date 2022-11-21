#include "header.h"

#define PINGGER true

#define WAIT true

#define DEBUG true

// Hardware serial
#define Rx D3
#define Tx D4
// nano
// #define Rx 5
// #define Tx 6
SoftwareSerial nano(Rx, Tx);
String serialMsg = "";
String pendingMsg;
bool fromSerial = false;
bool serialAvilable = true;
bool connectedToNano = false;

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
    while (nano.available())
    {
        pendingMsg += nano.readString();
        fromSerial = false;
    }
    bool msgPending = breakeble();
    if (msgPending)
        Serial.println(String(F("Incoming from ")) + (fromSerial ? F("Serial") : F("nano")) + F(" :{\n ") + serialMsg + F("\n}\n "));
    if (pendingMsg == "\0")
        pendingMsg = "";
    return msgPending;
}

// private debug
void debugSend(String &msg, bool fromSerial = false)
{
    Serial.println(fromSerial ? String(F("message: ")) + msg : String(F("sending to nano:{\n")) + msg + F("\n}"));
}

void stream(String msg, int size)
{
    Serial.println(F("Streaming to nano..."));
    nano.println(F("stream"));
    nano.println(F("S"));
    nano.println(size);
    nano.println(msg);
    debugSend(msg);
}

void sendCom(String msg, int size, bool toSerial = false)
{
    Serial.println(String(F("pending message with size of ")) + String(size) + F(" bytes"));
    if (!toSerial)
        nano.println(size);
    while (!readSerial() && serialMsg[0] != '!')
        ;
    Serial.print("response is:");
    Serial.println(serialMsg);
    if (!toSerial)
        nano.println(msg);
    debugSend(msg, toSerial);
}

// initialize serial
void SerialInit()
{
    Serial.println(F("hello from nano"));
    delay(1000);
    do
    {
        Serial.println(F("connecting to nano..."));
        nano.begin(9600);
        delay(1000);
    } while (!nano);
#if PINGGER
    sendCom(basicRes(F("ping")), 48);
    connectedToNano = true;
#endif
    // sendCom();
#if WAIT
    while (!connectedToNano)
        SerialRun();
    Serial.println(F("\nconnected to nano\n"));
#endif
}

// run serial
void SerialRun()
{
    /* Serial Comunication*/ {
        if (readSerial())
        {

            if (!fromSerial && !serialAvilable)
                return;
            bool isStream = serialMsg[0] == 'S';
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
                    nano.println(F("!"));
                // wait for the data
                while (!readSerial())
                    ;
                Serial.println(F("done waiting!"));
                Serial.println(serialMsg);
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
                sendCom(basicRes("pong"), 48, fromSerial);
                serialAvilable = true;
                Serial.println(F("serial com activated from ping"));
#if !PINGGER
                connectedToNano = true;
#endif
                return;
            }
            if (doc[F("type")] == F("disconnect!"))
            {
                sendCom(basicRes(F("disconnectForward")), 68);
                goto disconnectSerial;
            }
            if (doc[F("type")] == F("disconnectForward"))
                goto disconnectSerial;
            if (doc[F("type")] == F("stream"))
            {
                if (doc[F("stream")] == F("sensors"))
                {
                    JsonObject data = doc[F("data")];
                    temp = data[F("suhu")].as<float>();
                    humidity = data[F("humidity")].as<float>();

                    flame = data[F("flame")].as<float>();
                    light = data[F("light")].as<float>();

                    JsonObject gas = data[F("gas")];
                    smoke = gas[F("smoke")].as<float>();
                    co = gas[F("co")].as<float>();
                    lpg = gas[F("lpg")].as<float>();

                    data[F("orang")] = jumlahOrang;

                    String out;
                    serializeJson(doc, out);
#if DEBUG
                    Serial.println(String(F("\nlpg:\t")) + String(lpg) +
                                   F("\nco:\t") + String(co) +
                                   F("\nsmoke:\t") + String(smoke) +
                                   F("\ntemp:\t") + String(temp) +
                                   F("\nhumid:\t") + String(humidity) +
                                   F("\nhIndex:\t") + String(hIndex) +
                                   F("\nflame:\t") + String(flame) +
                                   F("\norang:\t") + String(jumlahOrang) +
                                   F("\nlight:\t") + String(light));
                    Serial.println(F("sending to web"));
#endif
                    sendWs(out);
                    return;
                }
            }
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
