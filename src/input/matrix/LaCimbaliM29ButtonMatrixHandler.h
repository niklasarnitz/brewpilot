#pragma once

#include "input/matrix/MachineButtonMatrixHandler.h"

class LaCimbaliM29ButtonMatrixHandler : public MachineButtonMatrixHandler
{
    static void selectScanLine(int pin);

public:
    LaCimbaliM29ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void initializePins();

    void handle();
};
