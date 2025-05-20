#include "ClickHandler.h"

ClickHandler::ClickHandler(unsigned long blockPeriodMs)
    : _blockPeriodMs(blockPeriodMs), _lastClickTime(0) {}

void ClickHandler::handleClick(ButtonCallback callback) {
    unsigned long now = millis();
    if (now - _lastClickTime >= _blockPeriodMs) {
        if (callback)
            callback();
        resetSleepTimer();
        _lastClickTime = now;
    }
}

void ClickHandler::resetSleepTimer() {
    // TODO: Implement sleep timer reset logic
}