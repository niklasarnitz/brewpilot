#include "input/InputHandler.h"

InputHandler::InputHandler(ButtonEvent *buttonEvent)
    : buttonEvent(buttonEvent), buttonMatrixHandler(&buttonMatrixState),
      teaButtonHandler(&buttonMatrixState.tea), groupOneHandler(&(buttonMatrixState.groupOne), &(buttonEvent->groupOne), 1), groupTwoHandler(&(buttonMatrixState.groupTwo), &(buttonEvent->groupTwo), 2) {}

void InputHandler::readInputs()
{
    // Read inputs
    buttonMatrixHandler.handle();

    // Write Events
    buttonEvent->tea = teaButtonHandler.getEvent().event;
    groupOneHandler.handle();
    groupTwoHandler.handle();
}
