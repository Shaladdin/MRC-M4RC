#include "header.h"

#define DEBUG true

volatile int jumlahOrang = 0;
volatile bool emptyRoom = false;

// room detection state
#define EMPTY 0
#define ENTERING 1
#define LEAVE 2
#define MIDLE 3
volatile int state = EMPTY;
#define isEMPTY (!ir Dalam && !ir Luar)
#define isMIXED (ir Dalam != ir Luar)
#define isMIDLE (ir Dalam && ir Luar)

// wich one on first
#define OUT false
#define IN true
volatile bool inFirst;

#define Dalam [0]
#define Luar [1]
volatile bool ir[2];
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

void ICACHE_RAM_ATTR RoomDetectionRun()
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
            {
                jumlahOrang--;
                if (jumlahOrang < 0)
                    jumlahOrang = 0;
            }
            emptyRoom = jumlahOrang == 0;
        }
        state = EMPTY;
    }

#if DEBUG
    printState();
#endif
}

void RoomDetectionInit()
{
    attachInterrupt(digitalPinToInterrupt(irDalam), RoomDetectionRun, CHANGE);
    attachInterrupt(digitalPinToInterrupt(irLuar), RoomDetectionRun, CHANGE);
}