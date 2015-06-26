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

  Serial.print( "Connecting to " );
  Serial.println( host );

  WiFiClient client;
  if( !client.connect( host, httpPort ) ) {
    Serial.println( "Connection failed" );
    return;
  }

  Serial.print( "Requesting URL: " );
  Serial.print( host );
  Serial.println( path );

  client.print( String( "GET " ) + path + " HTTP/1.1\r\n" +
    "Host:" + host + "\r\n" +
    "Connection: close\r\n\r\n" );
  delay( 1000 );

  while( client.available() ) {
    String line = client.readStringUntil( '\r' );
    Serial.print( line );
  }

  Serial.println( "-" );
  Serial.println( "closing connection" );
  delay( 5000 );  
}

