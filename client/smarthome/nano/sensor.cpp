#include "header.h"

#define DEBUG true

// sensors object
MQ2 sensorGas(mq2Pin);
DHT sensorSuhu(dhtPin, DHT11);

// sensors reading
float lpg, co, smoke,
    temp, humidity, hIndex;

void SensorInit()
{
    // initialize sensors
    sensorGas.begin();
    sensorSuhu.begin();
}

void SensorRun()
{
    // Sensors read
    lpg = sensorGas.readLPG();
    co = sensorGas.readCO();
    smoke = sensorGas.readSmoke();
    temp = sensorSuhu.readTemperature();
    humidity = sensorSuhu.readHumidity();
    hIndex = sensorSuhu.computeHeatIndex(temp, humidity);
#if DEBUG
    // debugging stuff
    Serial.println(String(F("\nlpg:\t")) + String(lpg) +
                   F("\nco:\t") + String(co) +
                   F("\nsmoke:\t") + String(smoke) +
                   F("\ntemp:\t") + String(temp) +
                   F("\nhumid:\t") + String(humidity) +
                   F("\nhIndex:\t") + String(hIndex));
#endif
}
