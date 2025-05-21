#include "EventService.h"
#include <esp_sleep.h>

EventService& EventService::instance() {
    static EventService instance;
    return instance;
}

EventService::EventService() : _lastActivityMs(millis()), _inactivitySleepMinutes(1) {}

void EventService::registerService(const String& name) {
    _serviceReady[name] = false;
}

void EventService::setServiceReadyToSleep(const String& name, bool ready) {
    _serviceReady[name] = ready;
}

void EventService::resetInactivityTimer() {
    _lastActivityMs = millis();
}

void EventService::scheduleCallback(uint32_t minutes, EventCallback callback) {
    unsigned long triggerTime = millis() + minutes * 60 * 1000;
    _scheduledEvents.push_back({triggerTime, callback});
}

void EventService::setInactivitySleepMinutes(uint32_t minutes) {
    _inactivitySleepMinutes = minutes;
}

void EventService::update() {
    checkScheduledEvents();
    checkSleep();
}

void EventService::checkScheduledEvents() {
    unsigned long now = millis();
    for (auto it = _scheduledEvents.begin(); it != _scheduledEvents.end(); ) {
        if (now >= it->first) {
            if (it->second) it->second();
            it = _scheduledEvents.erase(it);
        } else {
            ++it;
        }
    }
}

void EventService::checkSleep() {
    unsigned long currentTimeMs = millis();

    // Check if inactivity threshold has been reached
    unsigned long inactivityThresholdMs = _inactivitySleepMinutes * 60 * 1000;
    if ((currentTimeMs - _lastActivityMs) < inactivityThresholdMs) return;

    // Ensure all services are ready before sleeping
    for (const auto& [serviceName, isReady] : _serviceReady) {
        if (!isReady) return;
    }

    Serial.println("Entering deep sleep due to inactivity...");
    delay(100);

    // Turn off onboard LED (GPIO 2)
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // Set up wakeup timer based on the closest scheduled event
    if (!_scheduledEvents.empty()) {
        // Find the earliest scheduled event time
        unsigned long earliestEventTimeMs = _scheduledEvents.front().first;
        for (const auto& [eventTime, _] : _scheduledEvents) {
            if (eventTime < earliestEventTimeMs) {
                earliestEventTimeMs = eventTime;
            }
        }

        // Calculate delay until next event
        unsigned long delayUntilEventMs = (earliestEventTimeMs > currentTimeMs)
            ? (earliestEventTimeMs - currentTimeMs)
            : 0;

        // Schedule wakeup if delay is greater than 0
        if (delayUntilEventMs > 0) {
            esp_sleep_enable_timer_wakeup(delayUntilEventMs * 1000); // Convert ms to µs
        }
    }

    esp_deep_sleep_start();
}

void EventService::printWakeupReason() {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO (button)"); break;
        case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
        case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
        case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
        default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
    }
}