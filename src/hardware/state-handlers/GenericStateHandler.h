//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_GENERICSTATEHANDLER_H
#define BREWPILOT_GENERICSTATEHANDLER_H

#include "hardware/state/State.h"

class GenericStateHandler
{
public:
    virtual void handleState() = 0;
};

#endif // BREWPILOT_GENERICSTATEHANDLER_H
