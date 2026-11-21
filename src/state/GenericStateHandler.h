#pragma once

#include "state/State.h"

class GenericStateHandler
{
public:
    virtual void handleState() = 0;
};
