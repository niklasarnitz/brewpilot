//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_GROUPBUTTONHANDLER_H
#define BREWPILOT_GROUPBUTTONHANDLER_H

#include "ButtonHandler.h"
#include "structs/ButtonMatrixState.h"
#include "hardware/enums/GroupHeadButtonEvent.h"

class GroupHeadButtonHandler {
private:
    // Output
    GroupHeadButtonEvent &event;

    ButtonHandler groupLeftSingleDoseButtonHandler;
    ButtonHandler groupLeftDoubleDoseButtonHandler;
    ButtonHandler groupContinuousButtonHandler;
    ButtonHandler groupRightSingleDoseButtonHandler;
    ButtonHandler groupRightDoubleDoseButtonHandler;

public:
    explicit GroupHeadButtonHandler(GroupButtonMatrixState &groupState, GroupHeadButtonEvent &event) :
            event(event),
            groupLeftSingleDoseButtonHandler(
                    groupState.leftSingleDose),
            groupLeftDoubleDoseButtonHandler(
                    groupState.leftDoubleDose),
            groupContinuousButtonHandler(
                    groupState.continuous),
            groupRightSingleDoseButtonHandler(
                    groupState.rightSingleDose),
            groupRightDoubleDoseButtonHandler(
                    groupState.rightDoubleDose) {};

    void handle() {
        bool leftSingleDoseEvent = groupLeftSingleDoseButtonHandler.handleButton();
        bool leftDoubleDoseEvent = groupLeftDoubleDoseButtonHandler.handleButton();
        bool continuousEvent = groupContinuousButtonHandler.handleButton();
        bool rightSingleDoseEvent = groupRightSingleDoseButtonHandler.handleButton();
        bool rightDoubleDoseEvent = groupRightDoubleDoseButtonHandler.handleButton();

        if (continuousEvent) {
            event = CONTINUOUS;
        } else if (leftSingleDoseEvent) {
            event = LEFT_SINGLE_ESPRESSO;
        } else if (leftDoubleDoseEvent) {
            event = LEFT_DOUBLE_ESPRESSO;
        } else if (rightSingleDoseEvent) {
            event = RIGHT_SINGLE_ESPRESSO;
        } else if (rightDoubleDoseEvent) {
            event = RIGHT_DOUBLE_ESPRESSO;
        } else {
            event = NONE;
        }
    }
};

#endif //BREWPILOT_GROUPBUTTONHANDLER_H
