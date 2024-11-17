//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_GROUPBUTTONHANDLER_H
#define BREWPILOT_GROUPBUTTONHANDLER_H

#include "ButtonHandler.h"
#include "structs/ButtonMatrixState.h"

class GroupHeadButtonHandler {
private:
    GroupButtonMatrixState groupState;

    ButtonHandler groupLeftSingleDoseButtonHandler;
    ButtonHandler groupLeftDoubleDoseButtonHandler;
    ButtonHandler groupContinuousButtonHandler;
    ButtonHandler groupRightSingleDoseButtonHandler;
    ButtonHandler groupRightDoubleDoseButtonHandler;

public:
    explicit GroupHeadButtonHandler(GroupButtonMatrixState groupState) : groupState(groupState),
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
};

#endif //BREWPILOT_GROUPBUTTONHANDLER_H
