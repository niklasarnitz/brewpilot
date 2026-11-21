#pragma once

#include "config/SelectedMachine.h"
#include "input/ButtonEvents.h"
#include "input/ButtonHandler.h"
#include "input/ButtonMatrixState.h"
#include "input/GroupHeadButtonHandler.h"
#include "input/matrix/ButtonMatrix.h"

class InputHandler
{
private:
    ButtonMatrixState buttonMatrixState{};

    ButtonMatrix buttonMatrix;

    GroupHeadButtonHandler groupOneHandler;
    GroupHeadButtonHandler groupTwoHandler;

    ButtonHandler teaButtonHandler;

    ButtonEvent &buttonEvent;

public:
    explicit InputHandler(ButtonEvent &buttonEvent);

    void readInputs();
};
