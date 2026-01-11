#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(bool *matrixButtonState) : matrixButtonState(matrixButtonState)
{
    buttonState.currentState = false;
}
ButtonState ButtonHandler::getEvent()
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
