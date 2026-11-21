#pragma once

#include "input/ButtonMatrixState.h"
#include "input/matrix/MachineButtonMatrixHandler.h"

class ButtonMatrixHandler
{
private:
    ButtonMatrixState *buttonMatrixState;
    MachineButtonMatrixHandler *machineButtonMatrixHandler;

public:
    ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void handle();
};
