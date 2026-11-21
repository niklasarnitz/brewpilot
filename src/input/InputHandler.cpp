#include "input/InputHandler.h"

InputHandler::InputHandler(ButtonEvent &buttonEvent)
    : buttonMatrix(buttonMatrixState),
      groupOneHandler(buttonMatrixState.groupOne, buttonEvent.groupOne, 1),
      groupTwoHandler(buttonMatrixState.groupTwo, buttonEvent.groupTwo, 2),
      teaButtonHandler(buttonMatrixState.tea),
      buttonEvent(buttonEvent)
{
    buttonMatrix.initializePins();
}

void InputHandler::readInputs()
{
    // Read inputs
    buttonMatrix.handle();

    // Write Events
    buttonEvent.tea = teaButtonHandler.getEvent().event;
    groupOneHandler.handle();
    groupTwoHandler.handle();
}
