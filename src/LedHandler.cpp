#include "LEDHandler.h"

LEDHandler::LEDHandler(uint8_t pin, bool activeHigh)
    : _pin(pin), _activeHigh(activeHigh) {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, !_activeHigh); // Ensure LED is off initially
}

void LEDHandler::turnOnForSeconds(uint16_t seconds) {
    digitalWrite(_pin, _activeHigh);
    delay(seconds * 1000);
    digitalWrite(_pin, !_activeHigh);
}