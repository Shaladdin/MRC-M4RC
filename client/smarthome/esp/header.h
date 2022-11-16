#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// outputs pin
#define lamp D3
#define fan D4
const int outputs[] = {lamp, fan, buzzer};

// ir
#define irDalam D5
#define irLuar D6
const int inputs[] = {irDalam, irLuar};

void pinInit();
// Software serial
extern SoftwareSerial nano;
void SerialInit();
void SerialRun();