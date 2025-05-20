#ifndef LEDHANDLER_H
#define LEDHANDLER_H

#include <Arduino.h>

class LEDHandler
{
public:
    LEDHandler(uint8_t pin, bool activeHigh = true);
    void turnOnForSeconds(uint16_t seconds);

private:
    uint8_t _pin;
    bool _activeHigh;
};

#endif