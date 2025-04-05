//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_BUTTONHANDLER_H
#define BREWPILOT_BUTTONHANDLER_H

#include "Arduino.h"

class ButtonHandler
{
private:
    bool lastState;

    bool *matrixButtonState;

public:
    explicit ButtonHandler(bool *matrixButtonState) : matrixButtonState(matrixButtonState)
    {
        lastState = false;
    };

    bool handleButton(bool foo = false)
    {
        bool returnValue = !lastState && *matrixButtonState;

        lastState = *matrixButtonState;

        return returnValue;
    }
};

#endif // BREWPILOT_BUTTONHANDLER_H
