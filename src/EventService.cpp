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
    unsigned long now = millis();
    if ((now - _lastActivityMs) > (_inactivitySleepMinutes * 60 * 1000)) {
        for (const auto& entry : _serviceReady) {
            if (!entry.second) return;
        }
        Serial.println("Entering deep sleep due to inactivity...");
        delay(100); // Allow serial print to flush

        esp_deep_sleep_start();
    }
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