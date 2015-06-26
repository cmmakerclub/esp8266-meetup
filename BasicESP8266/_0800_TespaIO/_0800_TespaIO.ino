#include <ESP8266WiFi.h>
#include <WiFiHelper.h>
#include <DHT.h>

const char* ssid     = "your_wifi_name";
const char* pass     = "your_wifi_password";

const char *host = "www.tespa.io";
const int httpPort = 80;

WiFiHelper *wifi;

#define PIN_VCC 16
#define PIN_GND 13
#define DHTPIN  14
#define DHTTYPE DHT22

DHT *dht;

void init_hardware()
{
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

void setup() {
  init_hardware();
  init_wifi();
}

void loop() {
  wifi->loop();

  float h = dht->readHumidity();
  float t = dht->readTemperature();

  Serial.print( "Humidity: " );
  Serial.print( h );
  Serial.print( "\t" );
  Serial.print( "Temperature: " );
  Serial.print( t );
  Serial.println( " C " );  
  
  String path = String("/testsensor/send/ESP8266_Meetup?temperature=") + String(t) + "&humidity=" + String(h) + 
    "&lattitude=18.783118045625173&longitude=98.97900942142633";

  Serial.println( wifi->get( host, path.c_str() ) );
  
  delay( 5000 );  
}

