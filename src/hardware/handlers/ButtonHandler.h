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

struct ButtonState
{
    ButtonEventType event = ButtonEventType::BUTTON_NONE;
    bool currentState = false;
};

class ButtonHandler
{
private:
    ButtonState buttonState = {};

    unsigned int pressedStreak = 0;

    bool *matrixButtonState;

public:
    explicit ButtonHandler(bool *matrixButtonState)
        : matrixButtonState(matrixButtonState)
    {
        buttonState.currentState = false;
    };

    ButtonState getEvent()
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
            buttonState.event = ButtonEventType::BUTTON_HELD;
        }
        else if (!buttonState.currentState && *matrixButtonState)
        {
            buttonState.event = ButtonEventType::BUTTON_PRESSED;
        }
        else
        {
            buttonState.event = ButtonEventType::BUTTON_NONE;
        }
        buttonState.currentState = *matrixButtonState;
        return buttonState;
    }
};

#endif // BREWPILOT_BUTTONHANDLER_H
