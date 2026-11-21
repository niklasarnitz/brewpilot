#pragma once

#include "input/ButtonEvents.h"

// With 50ms loop delay in main.cpp, MAX_STREAK=40 means ~2 seconds to register a held button
constexpr unsigned int MAX_STREAK = 40;

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
