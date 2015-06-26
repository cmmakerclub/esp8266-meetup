#include <ESP8266WiFi.h>
#include "DHT.h"

#define PIN_VCC 16
#define PIN_GND 13
#define DHTPIN  14
#define DHTTYPE DHT22

DHT *dht;

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200 );

  pinMode(PIN_VCC, OUTPUT);
  pinMode(PIN_GND, OUTPUT);
  digitalWrite( PIN_VCC, HIGH );
  digitalWrite( PIN_GND, LOW );

  dht = new DHT( DHTPIN, DHTTYPE, 15 );
  
  dht->begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  float h = dht->readHumidity();
  float t = dht->readTemperature();

  Serial.print( "Humidity: " );
  Serial.print( h );
  Serial.print( "\t" );
  Serial.print( "Temperature: " );
  Serial.print( t );
  Serial.println( " C " );

  // Repeat every 10 seconds
  delay(10000);
}
