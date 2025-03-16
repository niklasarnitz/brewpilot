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

class InputHandler {
private:
    ButtonMatrixState buttonMatrixState{};

    ButtonMatrixHandler buttonMatrixHandler;

    GroupHeadButtonHandler groupOneHandler;
    GroupHeadButtonHandler groupTwoHandler;

    // Tea
    ButtonHandler teaButtonHandler;


    ButtonEvent &buttonEvent;
//    TODO: Implement the reading of the button matrix here using coordinates

public:
    explicit InputHandler(ButtonEvent &buttonEvent) :
            buttonEvent(buttonEvent), buttonMatrixHandler(buttonMatrixState),
            teaButtonHandler(buttonMatrixState.tea), groupOneHandler(
            buttonMatrixState.groupOne, buttonEvent.groupOne),
            groupTwoHandler(buttonMatrixState.groupTwo, buttonEvent.groupTwo) {};


    void readInputs() {
        // Read inputs
        buttonMatrixHandler.handle();

        // Write Events
        teaButtonHandler.handleButton();
        groupOneHandler.handle();
        groupTwoHandler.handle();
    };
};


#endif //BREWPILOT_INPUTHANDLER_H
