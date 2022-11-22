#include "header.h"
bool securityMode = false;
volatile int pendingNotif = false;

#define securityAlert 1
#define gasAlert 2
#define flameAlert 3

void SecurityRun()
{
    // if intruder come in
    if (securityMode && !emptyRoom)
        pendingNotif = securityAlert;

    // if fire detected
    if (flame > maxFlame)
        pendingNotif = flameAlert;

    // if unusual gas detected
    float gas[] = {lpg, co, smoke};
    for (float x : gas)
        if (x > maxGas)
        {
            pendingNotif = gasAlert;
            break;
        }

    NotifRun();
}

void NotifRun()
{
    if (!pendingNotif)
        return;
    // send it to websocket
    String out;
    StaticJsonDocument<64> doc;
    doc[F("type")] = F("notify");
    doc[F("notify")] = pendingNotif;
    pendingNotif = false;
    tone(buzzer, 1000);
    serializeJson(doc, out);
    sendWs(out);
}