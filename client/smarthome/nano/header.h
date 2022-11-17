#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <MQ2.h>
#include <DHT.h>

const int outputs[] = {};

// Input pins
#define apiPin A7
const int inputs[] = {apiPin};

void pinInit();

// Sensors
#define mq2Pin A6
#define dhtPin 2
extern MQ2 sensorGas;
extern DHT sensorSuhu;
void SensorInit();
void SensorRun();

// sensors reading
extern float lpg, co, smoke,
    temp, humidity, hIndex;

// Serial Comunication
extern SoftwareSerial esp;
extern bool connectedToEsp;
void SerialInit();
void SerialRun();
