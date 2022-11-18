#include "header.h"

#define DEBUG true

// sensors object
MQ2 sensorGas(mq2Pin);
DHT sensorSuhu(dhtPin, DHT11);

// sensors reading
float lpg, co, smoke,
    temp, humidity, hIndex,
    flame, light;

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
    flame = mapF(analogRead(apiPin), 1023, 0, 0, 100);
    light = mapF(analogRead(ldrPin), 1023, 0, 0, 100);
#if DEBUG
    // debugging stuff
    Serial.println(String(F("\nlpg:\t")) + String(lpg) +
                   F("\nco:\t") + String(co) +
                   F("\nsmoke:\t") + String(smoke) +
                   F("\ntemp:\t") + String(temp) +
                   F("\nhumid:\t") + String(humidity) +
                   F("\nhIndex:\t") + String(hIndex) +
                   F("\nflame:\t") + String(flame) +
                   F("\nlight:\t") + String(light));
    Serial.println(F("Sending to esp..."));
#endif

    // send it to esp
    // Stream it to server
    String streamStr;
    StaticJsonDocument<256> doc;

    doc[F("type")] = F("stream");
    doc[F("stream")] = F("sensors");

    JsonObject data = doc.createNestedObject(F("data"));
    data[F("suhu")] = temp;
    data[F("humidity")] = humidity;
    data[F("flame")] = flame;
    data[F("light")] = light;

    JsonObject data_gas = data.createNestedObject(F("gas"));
    data_gas[F("smoke")] = smoke;
    data_gas[F("co")] = co;
    data_gas[F("lpg")] = lpg;

    serializeJson(doc, streamStr);
    stream(streamStr, 256);
    delay(500);
}