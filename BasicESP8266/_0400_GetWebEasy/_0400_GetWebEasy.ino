#include <ESP8266WiFi.h>
#include <WiFiHelper.h>

const char* ssid     = "your_wifi_name";
const char* pass     = "your_wifi_password";

const char *host = "www.tespa.io";
const int httpPort = 80;
const char *path = "/test";

WiFiHelper *wifi;

void init_hardware()
{
  Serial.begin(115200);
  
  delay(10);
  Serial.println();
  Serial.println();
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

void setup() {
  init_hardware();
  init_wifi();
}

void loop() {
  wifi->loop();

  Serial.println( "Connecting..." );
  Serial.println( wifi->get( host, path ) );
  
  delay( 5000 );  
}

