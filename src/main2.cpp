/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-multiple-button
 */

#include <ezButton.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "time.h"
#include <PubSubClient.h>

const char* ssid = "IoT_H3/4";
const char* password = "98806829";

const char* mqttUser = "esp32";
const char* mqttPassword = "1234";
const char* mqtt2User = "device03";
const char* mqtt2Password = "device03-password";
const char* mqtt2TopicFeedback = "sensor/device03/feedback";
const char* mqtt2TopicTime = "sensor/device03/time";

const char* mqttTopicOutput = "esp32/output";
const char* mqttTopicFeedback = "esp32/feedback";
const char* mqttTopicTime = "esp32/time";

const char* ntpServer = "0.dk.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* mqtt_server = "192.168.0.204";
const char* mqtt_server2 = "192.168.0.130";
//WiFiClientSecure espClient;
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
      client.publish(mqttTopicFeedback, "Happy");
      break;
    case 2:
      Serial.println("Button 2 pressed");
      client.publish(mqttTopicFeedback, "Less Happy");
      break;
    case 3:
      Serial.println("Button 3 pressed");
      client.publish(mqttTopicFeedback, "Unhappy");
      break;
    case 4:
      Serial.println("Button 4 pressed");
      client.publish(mqttTopicFeedback, "Malding!");
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

void reconnect(const char* localMqttUser, const char* localMqttPassword, const char* localMqttTopicOutput) {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", localMqttUser, localMqttPassword)) {
      Serial.println("connected");
      client.subscribe(localMqttTopicOutput);
      client.subscribe(mqtt2TopicTime);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
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

  const char* ca_cert = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDETCCAfmgAwIBAgIUeIE1RAnzhwmWd9aBpLAa+spPlGwwDQYJKoZIhvcNAQEL\n" \
    "BQAwGDEWMBQGA1UEAwwNTXlNb3NxdWl0dG9DQTAeFw0yNTA1MjEwODI4NDhaFw0y\n" \
    "ODAzMTAwODI4NDhaMBgxFjAUBgNVBAMMDU15TW9zcXVpdHRvQ0EwggEiMA0GCSqG\n" \
    "SIb3DQEBAQUAA4IBDwAwggEKAoIBAQCxGNgZS59W2+WdV38cbp3Bt0SnCV+AvkN0\n" \
    "NlGdsA5WnUtueCUhhHxObVZNk2+vwn+K5FVUCBuMxOj/boXsckVXwG6c0vvMo63n\n" \
    "wXQeD1Wt0Bp3PiUVOO17BONdU2jCns3aPT0B02+JACnu7FtlebA5ajOZzFSYSAUC\n" \
    "yARgBj5mm4mE0LsrzwvjqnctK3554xY3SPn9GjnxYg2jLomfGMsgnlI9iXj0Quoz\n" \
    "8l86LuRYvX3XK30g2EYRIArQgl4DNrhtjiOzpsx6vnHEgW+TzYGnGpWuZk8Qf47W\n" \
    "zAr9YdUJ6JupBL+2G0DcJ+xuavbP0+QEl6z761XUr7Nbqbxq8hLjAgMBAAGjUzBR\n" \
    "MB0GA1UdDgQWBBQ30bsSszrDI9BsfTVNsTbEV5QPLzAfBgNVHSMEGDAWgBQ30bsS\n" \
    "szrDI9BsfTVNsTbEV5QPLzAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUA\n" \
    "A4IBAQAS2e114vsHyv+YHHRJ0lxXL18bnPNMdN11+Pj0VhxrJOJu+ZEmUjrXmGgt\n" \
    "LwmnON0dj6MNyWTwq+NfoVaYnNhmWBfIZ0i6jlwPwb1GbuRuy6YjIRbRCg8Dwcpm\n" \
    "UjflEGT0IM/Gp5n+VlF8urkqxNMcBA0vtcaAw8+3utjya1qbaHLQb+JOhfzmHj9P\n" \
    "wP6XtYT3Abbijm0FMCu3l/JdO80DybqO7DLU2MR+EnitLGXt7a+MgJb9FD6IO9oc\n" \
    "oIFUu2i1bN/gniXE6I76qHMI1y//WtMK+bEkfu4nF0SiI9YzP/Snt/j9D9LFR1RD\n" \
    "wm8FeAegca9T+X9cxoGYhW/4FIyP\n" \
    "-----END CERTIFICATE-----\n";

    const char* ESP_CA_cert = \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIDETCCAfmgAwIBAgIUeIE1RAnzhwmWd9aBpLAa+spPlGwwDQYJKoZIhvcNAQEL\n" \
  "BQAwGDEWMBQGA1UEAwwNTXlNb3NxdWl0dG9DQTAeFw0yNTA1MjEwODI4NDhaFw0y\n" \
  "ODAzMTAwODI4NDhaMBgxFjAUBgNVBAMMDU15TW9zcXVpdHRvQ0EwggEiMA0GCSqG\n" \
  "SIb3DQEBAQUAA4IBDwAwggEKAoIBAQCxGNgZS59W2+WdV38cbp3Bt0SnCV+AvkN0\n" \
  "NlGdsA5WnUtueCUhhHxObVZNk2+vwn+K5FVUCBuMxOj/boXsckVXwG6c0vvMo63n\n" \
  "wXQeD1Wt0Bp3PiUVOO17BONdU2jCns3aPT0B02+JACnu7FtlebA5ajOZzFSYSAUC\n" \
  "yARgBj5mm4mE0LsrzwvjqnctK3554xY3SPn9GjnxYg2jLomfGMsgnlI9iXj0Quoz\n" \
  "8l86LuRYvX3XK30g2EYRIArQgl4DNrhtjiOzpsx6vnHEgW+TzYGnGpWuZk8Qf47W\n" \
  "zAr9YdUJ6JupBL+2G0DcJ+xuavbP0+QEl6z761XUr7Nbqbxq8hLjAgMBAAGjUzBR\n" \
  "MB0GA1UdDgQWBBQ30bsSszrDI9BsfTVNsTbEV5QPLzAfBgNVHSMEGDAWgBQ30bsS\n" \
  "szrDI9BsfTVNsTbEV5QPLzAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUA\n" \
  "A4IBAQAS2e114vsHyv+YHHRJ0lxXL18bnPNMdN11+Pj0VhxrJOJu+ZEmUjrXmGgt\n" \
  "LwmnON0dj6MNyWTwq+NfoVaYnNhmWBfIZ0i6jlwPwb1GbuRuy6YjIRbRCg8Dwcpm\n" \
  "UjflEGT0IM/Gp5n+VlF8urkqxNMcBA0vtcaAw8+3utjya1qbaHLQb+JOhfzmHj9P\n" \
  "wP6XtYT3Abbijm0FMCu3l/JdO80DybqO7DLU2MR+EnitLGXt7a+MgJb9FD6IO9oc\n" \
  "oIFUu2i1bN/gniXE6I76qHMI1y//WtMK+bEkfu4nF0SiI9YzP/Snt/j9D9LFR1RD\n" \
  "wm8FeAegca9T+X9cxoGYhW/4FIyP\n" \
  "-----END CERTIFICATE-----";

const char* ESP_RSA_key =
  "-----BEGIN RSA PRIVATE KEY-----\n" \
  "MIIBOgIBAAJBAKj34GkxFhD90vcNLYLInFEX6Ppy1tPf9Cnzj4p4WGeKLs1Pt8Qu\n" \
  "KUpRKfFLfRYC9AIKjbJTWit+CqvjWYzvQwECAwEAAQJAIJLixBy2qpFoS4DSmoEm\n" \
  "o3qGy0t6z09AIJtH+5OeRV1be+N4cDYJKffGzDa88vQENZiRm0GRq6a+HPGQMd2k\n" \
  "TQIhAKMSvzIBnni7ot/OSie2TmJLY4SwTQAevXysE2RbFDYdAiEBCUEaRQnMnbp7\n" \
  "9mxDXDf6AU0cN/RPBjb9qSHDcWZHGzUCIG2Es59z8ugGrDY+pxLQnwfotadxd+Uy\n" \
  "v/Ow5T0q5gIJAiEAyS4RaI9YG8EWx/2w0T67ZUVAw8eOMB6BIUg0Xcu+3okCIBOs\n" \
  "/5OiPgoTdSy7bcF9IGpSE8ZgGKzgYQVZeN97YE00\n" \
  "-----END RSA PRIVATE KEY-----\n";
  
  Serial.println("Using certificate:");
  Serial.print(ca_cert);

  // espClient.setCACert(ca_cert);
  // espClient.setCertificate(ESP_CA_cert);
  // espClient.setPrivateKey(ESP_RSA_key); 
  //espClient.setInsecure();
  //client.setServer(mqtt_server, 8883);
  client.setServer(mqtt_server2, 1883);

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
    reconnect(mqtt2User, mqtt2Password, mqtt2TopicFeedback);
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
    client.publish(mqtt2TopicTime, timeStr);
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