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
    explicit GroupHeadButtonHandler(GroupButtonMatrixState *groupState, GroupHeadButtonEvent *event, int groupNumber, bool *isInProgrammingMode)
        : event(event),
          leftSingleDoseButtonHandler(
              &(groupState->leftSingleDose)),
          leftDoubleDoseButtonHandler(
              &(groupState->leftDoubleDose)),
          continuousButtonHandler(
              &groupState->continuous),
          rightSingleDoseButtonHandler(
              &groupState->rightSingleDose),
          rightDoubleDoseButtonHandler(
              &groupState->rightDoubleDose),
          groupNumber(groupNumber), isInProgrammingMode(isInProgrammingMode) {};

    void handle()
    {
        ButtonEventType leftSingleDoseEvent = leftSingleDoseButtonHandler.getEvent();
        ButtonEventType leftDoubleDoseEvent = leftDoubleDoseButtonHandler.getEvent();
        ButtonEventType continuousEvent = continuousButtonHandler.getEvent();
        ButtonEventType rightSingleDoseEvent = rightSingleDoseButtonHandler.getEvent();
        ButtonEventType rightDoubleDoseEvent = rightDoubleDoseButtonHandler.getEvent();

        // Held events are emitted for all buttons but only handled for the continuous button
        if (continuousEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::CONTINUOUS;
            Serial.printf("GroupHeadButtonHandler %d: continuous\n", groupNumber);
        }
        else if (continuousEvent == ButtonEventType::BUTTON_HELD)
        {
            *event = GroupHeadButtonEvent::CONTINUOUS_HELD;
            Serial.printf("GroupHeadButtonHandler %d: continuous held\n", groupNumber);
        }
        else if (leftSingleDoseEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: left single\n", groupNumber);
        }
        else if (leftDoubleDoseEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: left double\n", groupNumber);
        }
        else if (rightSingleDoseEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: right single\n", groupNumber);
        }
        else if (rightDoubleDoseEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: right double\n", groupNumber);
        }
        else
        {
            *event = GroupHeadButtonEvent::NONE;
        }
    }
};

#endif // BREWPILOT_GROUPBUTTONHANDLER_H
