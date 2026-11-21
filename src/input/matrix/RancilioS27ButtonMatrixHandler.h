#pragma once

#include "input/ButtonMatrixState.h"

class RancilioS27ButtonMatrixHandler
{
    ButtonMatrixState &buttonMatrixState;

public:
    explicit RancilioS27ButtonMatrixHandler(ButtonMatrixState &buttonMatrixState);

    void initializePins();

    void handle();
};
