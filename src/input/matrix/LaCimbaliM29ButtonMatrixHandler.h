#pragma once

#include <stdint.h>

#include "input/ButtonMatrixState.h"

class LaCimbaliM29ButtonMatrixHandler
{
    ButtonMatrixState *buttonMatrixState;

    static void selectScanLine(uint8_t pin);

public:
    LaCimbaliM29ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void initializePins();

    void handle();
};
