// connect all file
#include "header.h"

// sensors reading
float lpg, co, smoke,
    temp, humidity, hIndex,
    flame, light;

void setup()
{
    Serial.begin(115200);
    pinInit();
    WebsocketInit();
    SerialInit();
    RoomDetectionInit();
}

void loop()
{
    SerialRun();
    WebsocketRun();
}

// Draft
///*stream sensor*/ {
//     // Sensors read
//     lpg = sensorGas.readLPG();
//     co = sensorGas.readCO();
//     smoke = sensorGas.readSmoke();
//     temp = sensorSuhu.readTemperature();
//     humidity = sensorSuhu.readHumidity();
//     hIndex = sensorSuhu.computeHeatIndex(temp, humidity);

//     // debugging stuff
//     Serial.println(F("\nlpg:\t") + String(lpg) +
//                    F("\nco:\t") + String(co) +
//                    F("\nsmoke:\t") + String(smoke) +
//                    F("\ntemp:\t") + String(temp) +
//                    F("\nhumid:\t") + String(humidity) +
//                    F("\nhIndex:\t") + String(hIndex));

//     // code stop sign
//     if (!activated)
//         return;
//     // code beloww only for when device is connected

//     // Stream it to server
//     String streamStr;
//     StaticJsonDocument<256> doc;

//     doc[F("type")] = F("stream");
//     doc[F("stream")] = F("sensors");

//     JsonObject data = doc.createNestedObject(F("data"));
//     data[F("suhu")] = temp;
//     data[F("humidity")] = humidity;

//     JsonObject data_gas = data.createNestedObject(F("gas"));
//     data_gas[F("smoke")] = smoke;
//     data_gas[F("co")] = co;
//     data_gas[F("lpg")] = lpg;

//     serializeJson(doc, streamStr);
//     sendWs(streamStr);
// }

// /*security*/ {
//     bool in = !digitalRead(irDalam) && !digitalRead(irLuar);
//     Serial.println("logic: " + in ? "true" : "false");
//     if (in && securityMode)
//     {
//         tone(buzzer, 1000);
//         String notifyStr;
//         StaticJsonDocument<64> doc;
//         doc[F("type")] = F("notify");
//         doc[F("notify")] = F("security");
//         serializeJson(doc, notifyStr);
//         sendWs(notifyStr);
//     }
//     if (!securityMode)
//         noTone(buzzer);
// }

// /*mode otomatis*/ {

// }

// /*room detection*/ {

// }