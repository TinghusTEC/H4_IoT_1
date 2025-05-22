#include <Arduino.h>
#include "LEDHandler.h"
#include "ClickHandler.h"
#include "EventService.h"
#include <esp_sleep.h>

RTC_DATA_ATTR int wakeup_count = 0;

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
void printVote(VoteType vote) {
    switch (vote) {
        case BAD: Serial.print("BAD"); break;
        case NEUTRAL: Serial.print("NEUTRAL"); break;
        case GOOD: Serial.print("GOOD"); break;
        case VERY_GOOD: Serial.print("VERY GOOD"); break;
    }
}

// Global variable to pass button pin to callback
int currentButtonPin = -1;

// Single ClickHandler for all votes (7s block period)
ClickHandler voteClickHandler(7000);

// Callback for ClickHandler
void buttonCallback() {
    if (currentButtonPin != -1) {
        for (int i = 0; i < numVotes; ++i) {
            if (voteMappings[i].buttonPin == currentButtonPin) {
                Serial.print("Vote cast: ");
                printVote(voteMappings[i].vote);
                Serial.print(" (Button GPIO ");
                Serial.print(voteMappings[i].buttonPin);
                Serial.print(", LED GPIO ");
                Serial.print(voteMappings[i].ledPin);
                Serial.println(")");
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