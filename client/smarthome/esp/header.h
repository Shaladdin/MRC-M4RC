#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
using namespace websockets;

// outputs pin
#define buzzer D0
#define lamp D3
#define fan D4
const int outputs[] = {lamp, fan, buzzer};

// ir
#define irDalam D5
#define irLuar D6
const int inputs[] = {irDalam, irLuar};

void pinInit();

// Websocket
void WebsocketInit();
void WebsocketRun();
extern bool activated;

// Security
extern bool securityMode;

// room detection
extern volatile int jumlahOrang;
void RoomDetectionInit();
void RoomDetectionRun();

// Software serial
extern SoftwareSerial nano;
extern bool connectedToNano;
void SerialInit();
void SerialRun();

// sensors variable
extern float lpg, co, smoke,
    temp, humidity, flame, light;

    // Mode Otomatis
    void
    OtomatisRun();