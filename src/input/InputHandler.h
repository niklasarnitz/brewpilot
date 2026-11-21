#pragma once

#include "input/ButtonEvents.h"
#include "input/ButtonHandler.h"
#include "input/ButtonMatrixState.h"
#include "input/GroupHeadButtonHandler.h"
#include "input/matrix/ButtonMatrixHandler.h"

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
    explicit InputHandler(ButtonEvent *buttonEvent);

    void readInputs();
};
