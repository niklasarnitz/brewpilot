#include "input/InputHandler.h"

InputHandler::InputHandler(ButtonEvent *buttonEvent)
    : buttonEvent(buttonEvent), buttonMatrix(&buttonMatrixState),
      teaButtonHandler(&buttonMatrixState.tea), groupOneHandler(&(buttonMatrixState.groupOne), &(buttonEvent->groupOne), 1), groupTwoHandler(&(buttonMatrixState.groupTwo), &(buttonEvent->groupTwo), 2)
{
    buttonMatrix.initializePins();
}

void InputHandler::readInputs()
{
    // Read inputs
    buttonMatrix.handle();

    // Write Events
    buttonEvent->tea = teaButtonHandler.getEvent().event;
    groupOneHandler.handle();
    groupTwoHandler.handle();
}
