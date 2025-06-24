//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_BUTTONHANDLER_H
#define BREWPILOT_BUTTONHANDLER_H

#include "Arduino.h"

// With 50ms loop delay in main.cpp, MAX_STREAK=40 means ~2 seconds to register a held button
const unsigned int MAX_STREAK = 40;

enum class ButtonEventType
{
    BUTTON_PRESSED,
    BUTTON_HELD,
    BUTTON_NONE
};

class ButtonHandler
{
private:
    bool lastState;

    unsigned int pressedStreak = 0;

    bool *matrixButtonState;

public:
    explicit ButtonHandler(bool *matrixButtonState)
        : matrixButtonState(matrixButtonState)
    {
        lastState = false;
    };

    ButtonEventType getEvent()
    {
        if (*matrixButtonState)
        {
            if (pressedStreak <= MAX_STREAK)
            {
                pressedStreak++;
            }
        }
        else
        {
            pressedStreak = 0;
        }

        if (pressedStreak == MAX_STREAK)
        {
            lastState = *matrixButtonState;
            return ButtonEventType::BUTTON_HELD;
        }

        if (!lastState && *matrixButtonState)
        {
            lastState = *matrixButtonState;
            return ButtonEventType::BUTTON_PRESSED;
        }

        lastState = *matrixButtonState;
        return ButtonEventType::BUTTON_NONE;
    }
};

#endif // BREWPILOT_BUTTONHANDLER_H
