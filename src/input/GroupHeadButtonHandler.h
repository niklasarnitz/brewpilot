#pragma once

#include "input/ButtonEvents.h"
#include "input/ButtonHandler.h"
#include "input/ButtonMatrixState.h"

class GroupHeadButtonHandler
{
private:
    GroupHeadButtonEvent &event;

    ButtonHandler leftSingleDoseButtonHandler;
    ButtonHandler leftDoubleDoseButtonHandler;
    ButtonHandler continuousButtonHandler;
    ButtonHandler rightSingleDoseButtonHandler;
    ButtonHandler rightDoubleDoseButtonHandler;

    int groupNumber;

public:
    GroupHeadButtonHandler(const GroupButtonMatrixState &groupState, GroupHeadButtonEvent &event, int groupNumber);

    void handle();
};
