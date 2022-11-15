#include <ArduinoJson.h>
#include <Arduino.h>

// outputs pin
#define buzzer D0
#define lamp D3
#define fan D4
const int outputs[] = {lamp, fan, buzzer};

// ir
#define irDalam D6
#define irLuar D5
const int inputs[] = {irDalam, irLuar};

void pinInit();

// room detection
extern volatile int jumlahOrang;
void RoomDetectionInit();
void RoomDetectionRun();
