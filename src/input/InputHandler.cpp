#include "input/InputHandler.h"

InputHandler::InputHandler(ButtonEvent &buttonEvent)
    : buttonMatrix(buttonMatrixState),
#if BREWPILOT_GROUP_ONE_ACTIVE
      groupOneHandler(buttonMatrixState.groupOne, buttonEvent.groupOne, 1),
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
      groupTwoHandler(buttonMatrixState.groupTwo, buttonEvent.groupTwo, 2),
#endif
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
#if BREWPILOT_GROUP_ONE_ACTIVE
    groupOneHandler.handle();
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
    groupTwoHandler.handle();
#endif
}
