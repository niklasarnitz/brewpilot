//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_INPUTHANDLER_H
#define BREWPILOT_INPUTHANDLER_H

#include "structs/ButtonEvent.h"
#include "hardware/handlers/ButtonHandler.h"
#include "hardware/handlers/GroupHeadButtonHandler.h"
#include "structs/ButtonMatrixState.h"
#include "hardware/handlers/ButtonMatrixHandler.h"

class InputHandler
{
private:
    ButtonMatrixState buttonMatrixState{};

    ButtonMatrixHandler buttonMatrixHandler;

    GroupHeadButtonHandler groupOneHandler;
    GroupHeadButtonHandler groupTwoHandler;

    ButtonHandler teaButtonHandler;

    ButtonEvent *buttonEvent;

public:
    explicit InputHandler(ButtonEvent *buttonEvent, bool *isInProgrammingMode) : buttonEvent(buttonEvent), buttonMatrixHandler(&buttonMatrixState),
                                                                                 teaButtonHandler(&buttonMatrixState.tea), groupOneHandler(&(buttonMatrixState.groupOne), &(buttonEvent->groupOne), 1, isInProgrammingMode), groupTwoHandler(&(buttonMatrixState.groupTwo), &(buttonEvent->groupTwo), 2, isInProgrammingMode) {};

    void readInputs()
    {
        // Read inputs
        buttonMatrixHandler.handle();

        // Write Events
        buttonEvent->tea = teaButtonHandler.getEvent() == ButtonEventType::BUTTON_PRESSED;
        groupOneHandler.handle();
        groupTwoHandler.handle();
    };
};

#endif // BREWPILOT_INPUTHANDLER_H
