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
    explicit ButtonHandler(bool *matrixButtonState);

    ButtonState getEvent();
};

#endif // BREWPILOT_BUTTONHANDLER_H
