/***************************************************

Written by Marco Schwartz for Open Home Automation.  
BSD license, all text above must be included in any redistribution

Based on the original sketches supplied with the ESP8266/Arduino 
implementation written by Ivan Grokhotkov      

****************************************************/

// Libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
 
// Credentials
String deviceId     = "558d922e66c539b940001471";             // * set your device id (will be the MQTT client username)
String deviceSecret = "BQuq30bxldUIUf34c90aeYD40orP+SOk";         // * set your device secret (will be the MQTT client password)
String clientId     = "Meetup_DHT22";             // * set a random string (max 23 chars, will be the MQTT client id)

// WiFi name & password
const char* ssid = "your_wifi_name";
const char* password = "your_wifi_password"; 

// Topics
String outTopic     = "devices/" + deviceId + "/set"; // * MQTT channel where physical updates are published
String inTopic      = "devices/" + deviceId + "/get"; // * MQTT channel where lelylan updates are received
 
// WiFi client
WiFiClient wifiClient;

// Pin & type
#define PIN_VCC 16
#define PIN_GND 13
#define DHTPIN  14
#define DHTTYPE DHT22
 
// Initialize DHT sensor
DHT *dht;
 
// MQTT server address
IPAddress server(178, 62, 108, 47);
 
// MQTT
PubSubClient client(server);

 
void setup() {
  Serial.begin(115200);
  delay(500);
  
  pinMode(PIN_VCC, OUTPUT);
  pinMode(PIN_GND, OUTPUT);
  digitalWrite( PIN_VCC, HIGH );
  digitalWrite( PIN_GND, LOW );

  dht = new DHT( DHTPIN, DHTTYPE, 15 );
  dht->begin();
 
  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  lelylanConnection();      // MQTT server connection
}
 
void loop() {

  // Keep connection open
  lelylanConnection();
  
  // Reading temperature and humidity
  int h = dht->readHumidity();
  
  // Read temperature as Celsius
  int t = dht->readTemperature();
  
  // Messages for MQTT
  String temperature  = "{\"properties\":[{\"id\":\"558d925e97111c8396000003\",\"value\":\"" + String(t) + "\"}]}";
  String humidity = "{\"properties\":[{\"id\":\"558d927197111cafbf000002\",\"value\":\"" + String(h) + "\"}]}";
 
  // Publish temperature
  client.publish(outTopic, (char *) temperature.c_str());
  delay(100);
  
  // Publish humidity
  client.publish(outTopic, (char *) humidity.c_str());
  delay(10000);
  
}
 
/* MQTT server connection */
void lelylanConnection() {
  // add reconnection logics
  if (!client.connected()) {
    // connection to MQTT server
    if (client.connect(MQTT::Connect(clientId)
               .set_auth(deviceId, deviceSecret))) {
      Serial.println("[PHYSICAL] Successfully connected with MQTT");
      lelylanSubscribe(); // topic subscription
    }
  }
  client.loop();
}

/* MQTT subscribe */
void lelylanSubscribe() {
  client.subscribe(inTopic);
}
 
/* Receive Lelylan message and confirm the physical change */
void callback(char* topic, byte* payload, unsigned int length) {

}
