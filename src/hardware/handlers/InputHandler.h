//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_INPUTHANDLER_H
#define BREWPILOT_INPUTHANDLER_H

#include "../../configs/MachineDefines.h"
#include "../../structs/ButtonEvent.h"
#include "ButtonHandler.h"
#include "GroupHeadButtonHandler.h"
#include "../../structs/ButtonMatrixState.h"
#include "MachineButtonMatrixHandler.h"

class InputHandler
{
private:
    ButtonMatrixState buttonMatrixState{};
    MachineButtonMatrixHandler buttonMatrixHandler;
    GroupHeadButtonHandler groupOneHandler;
#if MACHINE_HAS_DUAL_GROUP
    GroupHeadButtonHandler groupTwoHandler;
#endif
    ButtonHandler teaButtonHandler;
    ButtonEvent *buttonEvent;

public:
    explicit InputHandler(ButtonEvent *buttonEvent, bool *isInProgrammingMode)
        : buttonEvent(buttonEvent),
          buttonMatrixHandler(&buttonMatrixState),
          teaButtonHandler(&buttonMatrixState.tea),
          groupOneHandler(&(buttonMatrixState.groupOne), &(buttonEvent->groupOne), 1, isInProgrammingMode)
#if MACHINE_HAS_DUAL_GROUP
          ,
          groupTwoHandler(&(buttonMatrixState.groupTwo), &(buttonEvent->groupTwo), 2, isInProgrammingMode)
#endif
    {};

    void readInputs()
    {
        // Read inputs with machine-specific optimization
        buttonMatrixHandler.handle();

        // Write Events
        buttonEvent->tea = teaButtonHandler.getEvent() == ButtonEventType::BUTTON_PRESSED;
        groupOneHandler.handle();

#if MACHINE_HAS_DUAL_GROUP
        groupTwoHandler.handle();
#endif
    };
};

#endif // BREWPILOT_INPUTHANDLER_H
