#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>

typedef void (*ButtonCallback)();

class ButtonHandler
{
public:
    ButtonHandler(int pin, ButtonCallback callback);
    void begin();

private:
    int _pin;
    ButtonCallback _callback;
    unsigned long _lastDebounceTime;
    unsigned long _debounceDelay;
};

#endif