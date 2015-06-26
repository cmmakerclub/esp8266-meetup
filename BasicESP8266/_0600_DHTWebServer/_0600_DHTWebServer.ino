#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <WiFiHelper.h>

const char* ssid     = "your_wifi_name";
const char* pass     = "your_wifi_password";

WiFiHelper *wifi;
ESP8266WebServer *server;

#define PIN_VCC 16
#define PIN_GND 13
#define DHTPIN  14
#define DHTTYPE DHT22

DHT *dht;

void init_hardware()
{››
  Serial.begin(115200);
  
  delay(10);
  Serial.println();
  Serial.println();

  pinMode(PIN_VCC, OUTPUT);
  pinMode(PIN_GND, OUTPUT);
  digitalWrite( PIN_VCC, HIGH );
  digitalWrite( PIN_GND, LOW );

  dht = new DHT( DHTPIN, DHTTYPE, 15 );
  dht->begin();  
}

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

void init_server()
{
  server = new ESP8266WebServer(80);
  
  server->on("/temperature", [](){
    int t = dht->readTemperature();
    server->send(200, "text/html", (String)(t) + " *C");
  });
  
  server->begin();
  Serial.println("HTTP server started");
}

void setup() {
  init_hardware();
  init_wifi();
  init_server();
}

void loop() {
  wifi->loop();
  server->handleClient();

  delay(1000);
}
