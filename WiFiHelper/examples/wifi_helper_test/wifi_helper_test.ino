// #define DEBUG_MODE

#include <ESP8266WiFi.h>
#include <WiFiHelper.h>

const char* ssid     = "CMMC.47";
const char* pass     = "guestnetwork";

WiFiHelper *wifi;

void init_wifi()
{
    wifi = new WiFiHelper(ssid, pass);

    wifi->on_connecting([](const char* message)
    {
        Serial.println (message);
    });

    wifi->on_connected([](const char* message)
    {
        Serial.println (message);
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());        
    });

    wifi->on_disconnected([](const char* message)
    {
        Serial.println (message);
    });
    
    wifi->begin();
}

void init_hardware()
{
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);
    delay(10);
    Serial.println();
    Serial.println();
}

void setup()
{
    init_hardware();
    init_wifi();
}

void loop()
{
    wifi->loop();
}