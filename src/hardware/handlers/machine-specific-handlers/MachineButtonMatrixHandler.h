//
// Created by Niklas Arnitz on 17.11.24.
//

#include "../../../structs/ButtonMatrixState.h"

#ifndef BREWPILOT_MACHINEBUTTONMATRIXHANDLER_H
#define BREWPILOT_MACHINEBUTTONMATRIXHANDLER_H

class MachineButtonMatrixHandler
{
protected:
    ButtonMatrixState *buttonMatrixState;

public:
    MachineButtonMatrixHandler(ButtonMatrixState *buttonMatrixState) : buttonMatrixState(buttonMatrixState) {}

    virtual void initializePins() = 0;

    virtual void handle() = 0;
};

#endif // BREWPILOT_MACHINEBUTTONMATRIXHANDLER_H
