//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_GROUPBUTTONHANDLER_H
#define BREWPILOT_GROUPBUTTONHANDLER_H

#include "ButtonHandler.h"
#include "structs/ButtonMatrixState.h"
#include "hardware/enums/GroupHeadButtonEvent.h"

class GroupHeadButtonHandler
{
private:
    GroupHeadButtonEvent *event;
    bool *isInProgrammingMode;
    ButtonHandler leftSingleDoseButtonHandler;
    ButtonHandler leftDoubleDoseButtonHandler;
    ButtonHandler continuousButtonHandler;
    ButtonHandler rightSingleDoseButtonHandler;
    ButtonHandler rightDoubleDoseButtonHandler;
    int groupNumber;

public:
    explicit GroupHeadButtonHandler(GroupButtonMatrixState *groupState, GroupHeadButtonEvent *event, int groupNumber, bool *isInProgrammingMode);
    void handle();
};

#endif // BREWPILOT_GROUPBUTTONHANDLER_H
