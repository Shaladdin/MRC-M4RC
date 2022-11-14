#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <MQ2.h>
#include <DHT.h>

// Serial Comunication
extern SoftwareSerial esp;
void SerialInit();
void SerialRun();
