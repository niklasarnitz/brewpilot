//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_BUTTONEVENT_H
#define BREWPILOT_BUTTONEVENT_H

#include "hardware/enums/GroupHeadButtonEvent.h"
#include "hardware/handlers/ButtonHandler.h"

struct ButtonEvent
{
    GroupHeadButtonEvent groupOne = GroupHeadButtonEvent::NONE;
    ButtonEventType tea = ButtonEventType::BUTTON_NONE;
    GroupHeadButtonEvent groupTwo = GroupHeadButtonEvent::NONE;
};

#endif // BREWPILOT_BUTTONEVENT_H
