#include <ESP8266WiFi.h>
#include <WebSocketClient.h>

const char *ssid = "Adzka";
const char *password = "123456789";
char path[] = "/echo";
char host[] = "demos.kaazing.com";

WebSocketClient ws;

// Use WiFiClient class to create TCP connections
WiFiClient wifi;

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(5000);

    // Connect to the websocket server
    Serial.println("connecting to websocket server");
    while (!wifi.connect(host, 80))
        Serial.println("connecting to websocket server failed");
    Serial.println("connected");
        
    // Handshake with the server
    ws.path = path;
    ws.host = host;
    while (!ws.handshake(wifi))
        Serial.println("Connection failed.");
}

void loop()
{
    String data;

    if (wifi.connected())
    {

        ws.getData(data);
        if (data.length() > 0)
        {
            Serial.print("Received data: ");
            Serial.println(data);
        }

        // capture the value of analog 1, send it along
        pinMode(1, INPUT);
        data = String(analogRead(1));

        ws.sendData(data);
    }
    else
    {
        Serial.println("Client disconnected.");
        while (1)
        {
            // Hang on disconnect.
        }
    }

    // wait to fully let the client disconnect
    delay(3000);
}
