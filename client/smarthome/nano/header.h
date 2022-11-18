#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <MQ2.h>
#include <DHT.h>

// common:
float mapF(float x, float in_min, float in_max, float out_min, float out_max);
template <typename T>
T diffrence(T num1, T num2);
template <typename T>
bool changed(T oldVar, T newVar, T index);

const int outputs[] = {};

// Input pins
#define apiPin A7
#define ldrPin A3
const int inputs[] = {apiPin, ldrPin};

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
    temp, humidity, hIndex,
    flame, light;

// Serial Comunication
extern SoftwareSerial esp;
extern bool connectedToEsp;
void SerialInit();
void SerialRun();
void sendCom(String msg, int size, bool toSerial = false);
void stream(String msg, int size);
bool readSerial();
