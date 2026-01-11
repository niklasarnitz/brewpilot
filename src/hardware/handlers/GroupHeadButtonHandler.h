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
          isInProgrammingMode(isInProgrammingMode),
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
          groupNumber(groupNumber) {};

    void handle()
    {
        ButtonState leftSingleDoseState = leftSingleDoseButtonHandler.getEvent();
        ButtonState leftDoubleDoseState = leftDoubleDoseButtonHandler.getEvent();
        ButtonState continuousState = continuousButtonHandler.getEvent();
        ButtonState rightSingleDoseState = rightSingleDoseButtonHandler.getEvent();
        ButtonState rightDoubleDoseState = rightDoubleDoseButtonHandler.getEvent();

        // Check for auto backflush combo: left single + right double simultaneously held
        if (((leftSingleDoseState.event == ButtonEventType::BUTTON_PRESSED) && rightDoubleDoseState.currentState) ||
            ((rightDoubleDoseState.event == ButtonEventType::BUTTON_PRESSED) && leftSingleDoseState.currentState))
        {
            *event = GroupHeadButtonEvent::AUTO_BACKFLUSH;
            Serial.printf("GroupHeadButtonHandler %d: auto backflush combo detected\n", groupNumber);
            return;
        }

        // Held events are emitted for all buttons but only handled for the continuous button
        if (continuousState.event == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::CONTINUOUS;
            Serial.printf("GroupHeadButtonHandler %d: continuous\n", groupNumber);
        }
        else if (continuousState.event == ButtonEventType::BUTTON_HELD)
        {
            *event = GroupHeadButtonEvent::CONTINUOUS_HELD;
            Serial.printf("GroupHeadButtonHandler %d: continuous held\n", groupNumber);
        }
        else if (leftSingleDoseState.event == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: left single\n", groupNumber);
        }
        else if (leftDoubleDoseState.event == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: left double\n", groupNumber);
        }
        else if (rightSingleDoseState.event == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO;
            Serial.printf("GroupHeadButtonHandler %d: right single\n", groupNumber);
        }
        else if (rightDoubleDoseState.event == ButtonEventType::BUTTON_PRESSED)
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
