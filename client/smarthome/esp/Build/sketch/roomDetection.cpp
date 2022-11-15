#line 1 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\esp\\roomDetection.cpp"
#include "header.h"

#define DEBUG true

int jumlahOrang = 0;
unsigned long int coolDown;
int waitTime = 500;

// room detection state
#define EMPTY 0
#define ENTERING 1
#define LEAVE 2
#define MIDLE 3
int state = EMPTY;
#define isEMPTY (!ir Dalam && !ir Luar)
#define isMIXED (ir Dalam != ir Luar)
#define isMIDLE (ir Dalam && ir Luar)

// wich one on first
#define OUT false
#define IN true
bool inFirst;

#define Dalam [0]
#define Luar [1]
bool ir[2];
String extra;
#if DEBUG
void printState()
{
    String out;
    switch (state)
    {
    case (EMPTY):
        out = F("empty");
        break;
    case (ENTERING):
        out = F("entering");
        break;
    case (LEAVE):
        out = F("leaving");
        break;
    case (MIDLE):
        out = F("midle");
        break;
    }
    Serial.println(out + "\t" +
                   String(ir Dalam ? F("on") : F("off")) + " " + String(ir Luar ? F("on") : F("off")) + "\t" +
                   String(inFirst ? "inside" : "outside") + " first\t" + String(jumlahOrang) + "\t" + extra);
}
#endif

void roomDetection()
{
    ir Dalam = !digitalRead(irDalam);
    ir Luar = !digitalRead(irLuar);

    // entering
    if (isMIXED && (state == EMPTY))
    {
        inFirst = ir Dalam;
        state = ENTERING;
        return;
    }
    if (isMIDLE)
        state = MIDLE;
    if (isMIXED && (state == MIDLE))
    {
        if (inFirst != ir Dalam)
            state = LEAVE;
    }
    if (isEMPTY)
    {
        if (state == LEAVE)
        {
            if (!inFirst)
                jumlahOrang++;
            else
                jumlahOrang--;
        }
        state = EMPTY;
    }
}

void RoomDetectionRun()
{
    if (coolDown > millis())
        return;
    roomDetection();
#if DEBUG
    printState();
#endif
}
