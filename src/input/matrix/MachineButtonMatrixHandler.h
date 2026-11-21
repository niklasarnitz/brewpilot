#pragma once

#include "input/ButtonMatrixState.h"

class MachineButtonMatrixHandler
{
protected:
    ButtonMatrixState *buttonMatrixState;

public:
    MachineButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    virtual void initializePins() = 0;

    virtual void handle() = 0;
};
