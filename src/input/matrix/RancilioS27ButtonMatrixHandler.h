#pragma once

#include "input/matrix/MachineButtonMatrixHandler.h"

class RancilioS27ButtonMatrixHandler : public MachineButtonMatrixHandler
{
public:
    RancilioS27ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void initializePins();

    void handle();
};
