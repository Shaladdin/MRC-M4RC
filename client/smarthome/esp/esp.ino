#include "header.h"

// outputs pin
#define buzzer D2
#define lamp D0
#define fan D1
const int outputs[] = {lamp, fan, buzzer};
// sensors reading
float lpg, co, smoke,
    temp, humidity, hIndex;

// inputs pin
// ir
#define irDalam D5
#define irLuar D6
const int inputs[] = {irDalam, irLuar};

// room detection
bool prevOut, preIn, netral = false;
int JumlahOrang = 0;

bool securityMode = false;

void setup()
{
    Serial.begin(115200);
    Serial.println(F("hello from esp"));
    delay(1000);
    // initialize outputs pins
    for (int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++)
        pinMode(outputs[i], OUTPUT);
    // initialize inputs pins
    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
        pinMode(inputs[i], INPUT);
    SerialInit();
}

void loop()
{
    SerialRun();
}