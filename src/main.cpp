#include <Arduino.h>
#include "LEDHandler.h"
#include "ClickHandler.h"
#include "EventService.h"
#include <esp_sleep.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "time.h"
#include <PubSubClient.h>

RTC_DATA_ATTR int wakeup_count = 0;

const char* ssid = "IoT_H3/4";
const char* password = "98806829";
const char* mqttUser = "device03";
const char* mqttPassword = "device03-password";
const char* mqttTopicFeedback = "sensor/device03/feedback";
const char* mqttTopicTime = "sensor/device03/time";

const char* ntpServer = "0.dk.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* mqtt_server = "192.168.0.130";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

// Voting types
enum VoteType { BAD, NEUTRAL, GOOD, VERY_GOOD };

// Button-to-LED mapping and vote type
struct ButtonVoteMap {
    int buttonPin;
    LEDHandler* led;
    VoteType vote;
    int ledPin;
};

// Create LEDHandler instances for each LED
LEDHandler ledBad(23, true);
LEDHandler ledNeutral(22, true);
LEDHandler ledGood(19, true);
LEDHandler ledVeryGood(18, true);

// Map buttons to LEDs and votes
ButtonVoteMap voteMappings[] = {
    {26, &ledBad, BAD, 23},
    {27, &ledNeutral, NEUTRAL, 22},
    {14, &ledGood, GOOD, 19},
    {13, &ledVeryGood, VERY_GOOD, 18}
};
const int numVotes = sizeof(voteMappings) / sizeof(voteMappings[0]);

// Helper to print vote type
String printVote(VoteType vote) {
    switch (vote) {
        case BAD: return "BAD";
        case NEUTRAL: return "NEUTRAL";
        case GOOD:return "GOOD";
        case VERY_GOOD: return "VERY GOOD";
    }
    return "UNKNOWN";
}

// Global variable to pass button pin to callback
int currentButtonPin = -1;

// Single ClickHandler for all votes (7s block period)
ClickHandler voteClickHandler(7000);

void blinkLED(int pin,  unsigned long duration) {
    pinMode(pin, OUTPUT);
    unsigned long current = millis();
    unsigned long waitTime = current + duration;
    while(current < waitTime)
        {
            digitalWrite(pin, HIGH);
            delay(100);
            digitalWrite(pin, LOW);
            delay(100);
            current = millis();
        }
}

void mqttReconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
            Serial.println("connected");
        } 
        else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        pinMode(2, OUTPUT);
        blinkLED(2, 5000);
        }
    }
}

// Callback for MQTT messages
void mqttCallback(const char* topic, char* payload) {
    if (!client.connected()) {
        mqttReconnect();
    }
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print(" payload: ");
    Serial.print(payload);
    Serial.println("] ");
    client.publish(topic, payload);
}

// Callback for ClickHandler
void buttonCallback() {
    if (currentButtonPin != -1) {
        for (int i = 0; i < numVotes; ++i) {
            if (voteMappings[i].buttonPin == currentButtonPin) {
                Serial.print("Vote cast: ");
                Serial.print(printVote(voteMappings[i].vote));
                Serial.print(" (Button GPIO ");
                Serial.print(voteMappings[i].buttonPin);
                Serial.print(", LED GPIO ");
                Serial.print(voteMappings[i].ledPin);
                Serial.println(")");
                mqttCallback(mqttTopicFeedback, (char*)printVote(voteMappings[i].vote).c_str());
                voteMappings[i].led->turnOnForSeconds(7);
                EventService::instance().resetInactivityTimer();
                break;
            }
        }
    }
}

void dummyCallback() {
    Serial.println("Dummy scheduled callback fired!");
    EventService::instance().resetInactivityTimer();
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup() {
    Serial.begin(115200);
    delay(100);

    wakeup_count++; // needed for battery operation to force deep sleep when woken by power on

    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    // If not first boot and wakeup reason is 0, go back to sleep
    if (wakeup_count > 1 && wakeup_reason == 0) {
        Serial.println("suspicious wakeup detected, going back to sleep.");
        pinMode(2, OUTPUT);
        digitalWrite(2, LOW);
        delay(100);
        esp_deep_sleep_start();
    }

    // Enable deep sleep wakeup on all vote buttons (EXT1)
    uint64_t mask = 0;
    for (int i = 0; i < numVotes; ++i) {
        pinMode(voteMappings[i].buttonPin, INPUT_PULLUP);
        mask |= (1ULL << voteMappings[i].buttonPin);
    }
    esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ALL_LOW);

    // Print the wakeup reason
    EventService::instance().printWakeupReason();

    Serial.println("Setup started");

    // Register services
    EventService::instance().registerService("button");
    EventService::instance().registerService("dummy");

    // Set both services ready to sleep
    EventService::instance().setServiceReadyToSleep("button", true);
    EventService::instance().setServiceReadyToSleep("dummy", true);

    // Set inactivity sleep to 1 minute
    EventService::instance().setInactivitySleepMinutes(1);

    // Schedule a dummy callback in 2 minutes
    EventService::instance().scheduleCallback(2, dummyCallback);

    // Connect to WiFi
    client.setServer(mqtt_server, 1883);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    pinMode(2, OUTPUT);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(250);
        digitalWrite(2, HIGH);
        delay(250);
        digitalWrite(2, LOW);
    }
    
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Set up ntp server for time synchronization
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
}

void loop() {
    // Check all vote buttons
    for (int i = 0; i < numVotes; ++i) {
        if (digitalRead(voteMappings[i].buttonPin) == LOW) {
            currentButtonPin = voteMappings[i].buttonPin;
            voteClickHandler.handleClick(buttonCallback);
            // Wait for button release to avoid multiple triggers
            while (digitalRead(voteMappings[i].buttonPin) == LOW) {
                delay(10);
            }
            currentButtonPin = -1;
        }
    }
    EventService::instance().update();
}