#pragma once

#include <stdint.h>

#include "input/matrix/MachineButtonMatrixHandler.h"

class LaCimbaliM29ButtonMatrixHandler : public MachineButtonMatrixHandler
{
    static void selectScanLine(uint8_t pin);

public:
    LaCimbaliM29ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void initializePins();

    void handle();
};
