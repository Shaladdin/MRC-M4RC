#include "header.h"
bool securityMode = false;
volatile bool pendingNotif = false;

#define securityAlert 0
#define gasAlert 1
#define flameAlert 2

void SecurityRun()
{
    if (!pendingNotif)
        return;
    pendingNotif = false;
    if (!securityMode)
    {
        noTone(buzzer);
        return;
    }
    if (emptyRoom)
        return;
    Serial.println(F("SECURITY BREACH!!!!"));
    tone(buzzer, 1000);
    String notify = F("{    \"type\":\"notify\",    \"notify\":\"security\"}");
    sendWs(notify);
}