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
    virtual void initializePins();

    virtual void handle();
};

#endif // BREWPILOT_MACHINEBUTTONMATRIXHANDLER_H
