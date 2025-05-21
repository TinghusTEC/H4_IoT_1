/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-multiple-button
 */

#include <ezButton.h>
#include <WiFi.h>
#include "time.h"
#include <PubSubClient.h>

const char* ssid = "IoT_H3/4";
const char* password = "98806829";

const char* ntpServer = "0.dk.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* mqtt_server = "192.168.0.204";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
bool isLedOn = false;

unsigned long led_currentTime = millis();
unsigned long led_previousTime = 0; 
const long led_timeoutTime = 1000;

#define BUTTON_PIN_1 25  
#define BUTTON_PIN_2 26  
#define BUTTON_PIN_3 27  
#define BUTTON_PIN_4 14
#define LED_OUT1 23
#define LED_OUT2 22 
#define LED_OUT3 21
#define LED_OUT4 19
#define ON_BOARD_LED 2 


ezButton button1(BUTTON_PIN_1); 
ezButton button2(BUTTON_PIN_2);
ezButton button3(BUTTON_PIN_3);
ezButton button4(BUTTON_PIN_4);

void ledOn(int pressed) {
  if (isLedOn)
  {
    return;
  }
  switch (pressed) {
    case 1:
      digitalWrite(LED_OUT1, HIGH);
      break;
    case 2:
      digitalWrite(LED_OUT2, HIGH);
      break;
    case 3:
      digitalWrite(LED_OUT3, HIGH);
      break;
    case 4:
      digitalWrite(LED_OUT4, HIGH); 
      break;
    default:
      break;
  }
  led_previousTime = millis();
  isLedOn = true;
}

void buttonPress(int button) {
  switch (button) {
    case 1:
      Serial.println("Button 1 pressed");
      client.publish("esp32/feedback", "Happy");
      break;
    case 2:
      Serial.println("Button 2 pressed");
      client.publish("esp32/feedback", "Less Happy");
      break;
    case 3:
      Serial.println("Button 3 pressed");
      client.publish("esp32/feedback", "Unhappy");
      break;
    case 4:
      Serial.println("Button 4 pressed");
      client.publish("esp32/feedback", "Malding!");
      break;
    default:
      break;
  }
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_OUT1,OUTPUT);
  pinMode(LED_OUT2,OUTPUT);
  pinMode(LED_OUT3,OUTPUT);
  pinMode(LED_OUT4,OUTPUT);
  pinMode(ON_BOARD_LED, OUTPUT);  
  button1.setDebounceTime(100);  
  button2.setDebounceTime(100);  
  button3.setDebounceTime(100); 
  button4.setDebounceTime(100); 

  client.setServer(mqtt_server, 1883);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  printLocalTime();
}

void loop() {
  button1.loop(); 
  button2.loop();
  button3.loop();
  button4.loop();


  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    Serial.println(timeStr);
    client.publish("esp32/time", timeStr);
  }

  if (button1.isPressed()){
    buttonPress(1);
    ledOn(1);
  }
  if (button2.isPressed()){
    buttonPress(2);
    ledOn(2);
  }
  if (button3.isPressed()){
    buttonPress(3);
    ledOn(3);
  }
  if (button4.isPressed()){
    buttonPress(4);
    ledOn(4);
  }
  
  led_currentTime = millis();
  if (led_currentTime - led_previousTime >= led_timeoutTime && isLedOn) {
    digitalWrite(LED_OUT1, LOW);
    digitalWrite(LED_OUT2, LOW);
    digitalWrite(LED_OUT3, LOW);
    digitalWrite(LED_OUT4, LOW);
    isLedOn = false;
  }
}