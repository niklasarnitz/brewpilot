//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_GROUPBUTTONHANDLER_H
#define BREWPILOT_GROUPBUTTONHANDLER_H

#include "ButtonHandler.h"
#include "structs/ButtonMatrixState.h"
#include "hardware/enums/GroupHeadButtonEvent.h"

extern void logMessage(const char *message);

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
        ButtonEventType leftSingleDoseEvent = leftSingleDoseButtonHandler.getEvent();
        ButtonEventType leftDoubleDoseEvent = leftDoubleDoseButtonHandler.getEvent();
        ButtonEventType continuousEvent = continuousButtonHandler.getEvent();
        ButtonEventType rightSingleDoseEvent = rightSingleDoseButtonHandler.getEvent();
        ButtonEventType rightDoubleDoseEvent = rightDoubleDoseButtonHandler.getEvent();

        // Held events are emitted for all buttons but only handled for the continuous button
        if (continuousEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::CONTINUOUS;
            char buffer[64];
            sprintf(buffer, "GroupHeadButtonHandler %d: continuous", groupNumber);
            logMessage(buffer);
        }
        else if (continuousEvent == ButtonEventType::BUTTON_HELD)
        {
            *event = GroupHeadButtonEvent::CONTINUOUS_HELD;
            char buffer[64];
            sprintf(buffer, "GroupHeadButtonHandler %d: continuous held", groupNumber);
            logMessage(buffer);
        }
        else if (leftSingleDoseEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO;
            char buffer[64];
            sprintf(buffer, "GroupHeadButtonHandler %d: left single", groupNumber);
            logMessage(buffer);
        }
        else if (leftDoubleDoseEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO;
            char buffer[64];
            sprintf(buffer, "GroupHeadButtonHandler %d: left double", groupNumber);
            logMessage(buffer);
        }
        else if (rightSingleDoseEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO;
            char buffer[64];
            sprintf(buffer, "GroupHeadButtonHandler %d: right single", groupNumber);
            logMessage(buffer);
        }
        else if (rightDoubleDoseEvent == ButtonEventType::BUTTON_PRESSED)
        {
            *event = GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO;
            char buffer[64];
            sprintf(buffer, "GroupHeadButtonHandler %d: right double", groupNumber);
            logMessage(buffer);
        }
        else
        {
            *event = GroupHeadButtonEvent::NONE;
        }
    }
};

#endif // BREWPILOT_GROUPBUTTONHANDLER_H
