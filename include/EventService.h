#ifndef EVENTSERVICE_H
#define EVENTSERVICE_H

#include <Arduino.h>
#include <map>
#include <vector>

typedef void (*EventCallback)();

class EventService {
public:
    static EventService& instance();

    void registerService(const String& name);
    void setServiceReadyToSleep(const String& name, bool ready);
    void resetInactivityTimer();
    void scheduleCallback(uint32_t minutes, EventCallback callback);
    void setInactivitySleepMinutes(uint32_t minutes);
    void update();
    void printWakeupReason();

private:
    EventService();
    std::map<String, bool> _serviceReady;
    std::vector<std::pair<unsigned long, EventCallback>> _scheduledEvents;
    unsigned long _lastActivityMs;
    uint32_t _inactivitySleepMinutes;

    void checkSleep();
    void checkScheduledEvents();
};

#endif