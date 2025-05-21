#include <Arduino.h>
#include "LEDHandler.h"
#include "ClickHandler.h"
#include "EventService.h"
#include <esp_sleep.h>

RTC_DATA_ATTR int wakeup_count = 0;

const int buttonPin = 0; // ESP32 built-in BOOT button
const int ledPin = 2;    // ESP32 built-in LED

LEDHandler led(ledPin, true);
ClickHandler clickHandler;

void dummyCallback() {
    Serial.println("Dummy scheduled callback fired!");
    EventService::instance().resetInactivityTimer();
}

void onButtonClick() {
    Serial.println("Button pressed");
    led.turnOnForSeconds(2);
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
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, LOW);
        delay(100);
        esp_deep_sleep_start();
    }

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); // Wake up on button press (active LOW)

    // Print the wakeup reason
    EventService::instance().printWakeupReason();

    pinMode(buttonPin, INPUT_PULLUP);
    Serial.println("Setup started");

    // Turn ON onboard LED after wakeup
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);

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
    if (digitalRead(buttonPin) == LOW) {
        clickHandler.handleClick(onButtonClick);
        while (digitalRead(buttonPin) == LOW) {
            delay(10);
        }
    }
    EventService::instance().update();
}