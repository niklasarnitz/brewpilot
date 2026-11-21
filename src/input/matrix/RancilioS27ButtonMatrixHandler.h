#pragma once

#include "input/ButtonMatrixState.h"

class RancilioS27ButtonMatrixHandler
{
    ButtonMatrixState *buttonMatrixState;

public:
    RancilioS27ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void initializePins();

    void handle();
};
