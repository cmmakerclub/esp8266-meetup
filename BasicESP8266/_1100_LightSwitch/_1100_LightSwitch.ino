/***************************************************

Written by Marco Schwartz for Open Home Automation.  
BSD license, all text above must be included in any redistribution

Based on the original sketches supplied with the ESP8266/Arduino 
implementation written by Ivan Grokhotkov      

****************************************************/

// Libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiHelper.h>
 
// Credentials
String deviceId     = "558d7ce766c539278a00130a";             // * set your device id (will be the MQTT client username)
String deviceSecret = "DKpwfKFofbmt0byxbb8rueMoaw8yq4sI";         // * set your device secret (will be the MQTT client password)
String clientId     = "ESP8266_MeetupLED";        // * set a random string (max 23 chars, will be the MQTT client id)

// WiFi name & password
const char* ssid = "your_wifi_name";
const char* pass = "your_wifi_password"; 
 
// Sketch logic
// See more: http://lelylan.github.io/types-dashboard-ng/#/
char* payloadOn  = "{\"properties\":[{\"id\":\"518be5a700045e1521000001\",\"value\":\"on\"}]}";
char* payloadOff = "{\"properties\":[{\"id\":\"518be5a700045e1521000001\",\"value\":\"off\"}]}";

// Topics
String outTopic     = "devices/" + deviceId + "/set"; // * MQTT channel where physical updates are published
String inTopic      = "devices/" + deviceId + "/get"; // * MQTT channel where lelylan updates are received

// MQTT server address
IPAddress server(178, 62, 108, 47);
 
// WiFi Client
WiFiHelper *wifi;
 
// MQTT
PubSubClient client(server);

// Pins
int outPin = 16; // led
int inPin = 14; // Switch
 
// Logic
int state = HIGH;     // current state of the output pin
int reading;          // current reading from the input pin
int previous = LOW;   // previous reading from the input pin
long time = 0;        // the last time the output pin was toggled
long debounce = 200;  // the debounce time, increase if the output flickers

int currentSwitch = HIGH;
int lastSwitch = HIGH;

// Callback
void callback(const MQTT::Publish& pub) {
  
  // Copy the payload content into a char*
  char* json;
  json = (char*) malloc(pub.payload_len() + 1);
  memcpy(json, pub.payload(), pub.payload_len());
  json[pub.payload_len()] = '\0';
 
  // Update the physical status and confirm the executed update
  boolean state;
  if (String(payloadOn) == String(json)) {
    Serial.println("[LELYLAN] Led turned on");
    lelylanPublish("on");
    state = HIGH;
  } else {
    Serial.println("[LELYLAN] Led turned off");
    lelylanPublish("off");
    state = LOW;
  }
 
  digitalWrite(outPin, state);
  free(json);
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
  Serial.begin(115200);
  delay(500);
 
  init_wifi();
  
  // Set callback
  client.set_callback(callback);
  
  // MQTT server connection
  lelylanConnection();      
  pinMode(outPin, OUTPUT);  // led pin setup
  pinMode(inPin, INPUT_PULLUP);
}
 
void loop() {
  // Keep connection open
  lelylanConnection();

  if( digitalRead(inPin) == HIGH ) {
    currentSwitch = HIGH;
  }
  else {
    currentSwitch = LOW;
  }
  if( currentSwitch != lastSwitch ) {
    lastSwitch = currentSwitch;
    if( currentSwitch == HIGH ) {
      Serial.println( "Switch" );
      int state = digitalRead(outPin);
      if( state == HIGH ) {
        digitalWrite(outPin, LOW);
        lelylanPublish("off");      
      }
      else {
        digitalWrite(outPin, HIGH);
        lelylanPublish("on");      
      }
    }
  }
}
 
/* MQTT server connection */
void lelylanConnection() {
  // add reconnection logics
  if (!client.connected()) {
    // connection to MQTT server
    if (client.connect(MQTT::Connect(clientId).set_auth(deviceId, deviceSecret))) {
      Serial.println("[PHYSICAL] Successfully connected with MQTT");
      lelylanSubscribe(); // topic subscription
    }
  }
  client.loop();
}
 
/* MQTT publish */
void lelylanPublish(char* value) {
  if (value == "on")
    client.publish(outTopic, payloadOn); // light on
  else
    client.publish(outTopic, payloadOff); // light off
}
 
/* MQTT subscribe */
void lelylanSubscribe() {
  client.subscribe(inTopic);
}

// 
