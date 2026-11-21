#pragma once

enum class GroupHeadButtonEvent
{
    LEFT_SINGLE_ESPRESSO,
    LEFT_DOUBLE_ESPRESSO,
    RIGHT_SINGLE_ESPRESSO,
    RIGHT_DOUBLE_ESPRESSO,
    CONTINUOUS,
    CONTINUOUS_HELD,
    AUTO_BACKFLUSH,
    NONE
};

enum class ButtonEventType
{
    BUTTON_PRESSED,
    BUTTON_HELD,
    BUTTON_NONE
};

struct ButtonEvent
{
    GroupHeadButtonEvent groupOne = GroupHeadButtonEvent::NONE;
    ButtonEventType tea = ButtonEventType::BUTTON_NONE;
    GroupHeadButtonEvent groupTwo = GroupHeadButtonEvent::NONE;
};
