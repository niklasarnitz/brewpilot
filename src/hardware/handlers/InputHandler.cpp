//
// Created by Niklas Arnitz on 11.11.24.
//

#include "InputHandler.h"

InputHandler::InputHandler(ButtonEvent *buttonEvent, bool *isInProgrammingMode)
    : buttonMatrixHandler(&buttonMatrixState),
      groupOneHandler(&(buttonMatrixState.groupOne), &(buttonEvent->groupOne), 1, isInProgrammingMode),
      groupTwoHandler(&(buttonMatrixState.groupTwo), &(buttonEvent->groupTwo), 2, isInProgrammingMode),
      teaButtonHandler(&buttonMatrixState.tea),
      buttonEvent(buttonEvent)
{
}

void InputHandler::readInputs()
{
    // Read inputs
    buttonMatrixHandler.handle();

    // Write Events
    buttonEvent->tea = teaButtonHandler.getEvent().event == ButtonEventType::BUTTON_PRESSED;
    groupOneHandler.handle();
    groupTwoHandler.handle();
}
