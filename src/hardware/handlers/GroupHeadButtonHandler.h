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
    // Output
    GroupHeadButtonEvent *event;

    ButtonHandler groupLeftSingleDoseButtonHandler;
    ButtonHandler groupLeftDoubleDoseButtonHandler;
    ButtonHandler groupContinuousButtonHandler;
    ButtonHandler groupRightSingleDoseButtonHandler;
    ButtonHandler groupRightDoubleDoseButtonHandler;

    int groupNumber;

public:
    explicit GroupHeadButtonHandler(GroupButtonMatrixState *groupState, GroupHeadButtonEvent *event, int groupNumber) : event(event),
                                                                                                                        groupLeftSingleDoseButtonHandler(
                                                                                                                            &(groupState->leftSingleDose)),
                                                                                                                        groupLeftDoubleDoseButtonHandler(
                                                                                                                            &(groupState->leftDoubleDose)),
                                                                                                                        groupContinuousButtonHandler(
                                                                                                                            &groupState->continuous),
                                                                                                                        groupRightSingleDoseButtonHandler(
                                                                                                                            &groupState->rightSingleDose),
                                                                                                                        groupRightDoubleDoseButtonHandler(
                                                                                                                            &groupState->rightDoubleDose),
                                                                                                                        groupNumber(groupNumber) {};

    void handle()
    {
        bool leftSingleDoseEvent = groupLeftSingleDoseButtonHandler.handleButton();
        bool leftDoubleDoseEvent = groupLeftDoubleDoseButtonHandler.handleButton();
        bool continuousEvent = groupContinuousButtonHandler.handleButton();
        bool rightSingleDoseEvent = groupRightSingleDoseButtonHandler.handleButton();
        bool rightDoubleDoseEvent = groupRightDoubleDoseButtonHandler.handleButton();

        if (continuousEvent)
        {
            *event = CONTINUOUS;
            Serial.printf("GroupHeadButtonHandler %d: continuous\n", groupNumber);
        }
        else if (leftSingleDoseEvent)
        {
            *event = LEFT_SINGLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: left single\n", groupNumber);
        }
        else if (leftDoubleDoseEvent)
        {
            *event = LEFT_DOUBLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: left double\n", groupNumber);
        }
        else if (rightSingleDoseEvent)
        {
            *event = RIGHT_SINGLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: right single\n", groupNumber);
        }
        else if (rightDoubleDoseEvent)
        {
            *event = RIGHT_DOUBLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: right double\n", groupNumber);
        }
        else
        {
            *event = NONE;
        }
    }
};

#endif // BREWPILOT_GROUPBUTTONHANDLER_H
