//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_BUTTONEVENT_H
#define BREWPILOT_BUTTONEVENT_H

#include "hardware/enums/GroupHeadButtonEvent.h"

struct ButtonEvent
{
    GroupHeadButtonEvent groupOne = NONE;
    bool tea = false;
    GroupHeadButtonEvent groupTwo = NONE;
};

#endif // BREWPILOT_BUTTONEVENT_H
