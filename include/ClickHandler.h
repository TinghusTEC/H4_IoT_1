#ifndef CLICKHANDLER_H
#define CLICKHANDLER_H

#include <Arduino.h>

typedef void (*ButtonCallback)();

class ClickHandler
{
public:
    static constexpr unsigned long DEFAULT_BLOCK_PERIOD_MS = 7000; // 7 seconds
    ClickHandler(unsigned long blockPeriodMs = DEFAULT_BLOCK_PERIOD_MS);
    void handleClick(ButtonCallback callback);

private:
    unsigned long _blockPeriodMs;
    unsigned long _lastClickTime;
    void resetSleepTimer();
};

#endif