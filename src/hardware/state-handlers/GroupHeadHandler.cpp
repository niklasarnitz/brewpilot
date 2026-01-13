//
// Created by Niklas Arnitz on 16.11.24.
//

#include "GroupHeadHandler.h"
#include <climits>

GroupHeadStateHandler::GroupHeadStateHandler(bool *isExtracting, GroupHeadButtonEvent *event, VolumetricsHelper *volumetricsHelper, bool *isInProgrammingMode, int groupNumber)
    : volumetricsHelper(volumetricsHelper), event(event), isExtracting(isExtracting), isInProgrammingMode(isInProgrammingMode), groupNumber(groupNumber)
{
}

void GroupHeadStateHandler::handleState()
{
    // Handle auto backflush mode
    if (isAutoBackflushing)
    {
        handleAutoBackflush();
        return; // Auto backflush takes priority over normal operations
    }

    // Check for auto backflush activation
    if (*event == GroupHeadButtonEvent::AUTO_BACKFLUSH)
    {
        startAutoBackflush();
        return;
    }

    // Normal extraction handling
    if (*isExtracting)
    {
        if (*event == GroupHeadButtonEvent::CONTINUOUS_HELD && targetPulses == LONG_MAX && !(*isInProgrammingMode))
        {
            Serial.printf("GroupHeadStateHandler %d: Entered Programming Mode\n", groupNumber);
            *isInProgrammingMode = true;

            *isExtracting = false;

            currentPulses = 0;
            targetPulses = 0;
        }
        else if (*event == GroupHeadButtonEvent::CONTINUOUS)
        {
            *isExtracting = false;

            if (*isInProgrammingMode)
            {
                volumetricsHelper->writeFlowMeterSetting(buttonToBeProgrammed, currentPulses);

                Serial.printf("GroupHeadStateHandler %d: Stored %ld target pulses. Left Programming Mode\n", groupNumber, currentPulses);
                *isInProgrammingMode = false;
            }
            else
            {
                Serial.printf("GroupHeadStateHandler %d: Cancelled Extraction at %ld of %ld pulses\n", groupNumber, currentPulses, targetPulses);
            }

            currentPulses = 0;
            targetPulses = 0;
        }
        else if (!(*isInProgrammingMode) && currentPulses >= targetPulses)
        {
            *isExtracting = false;

            Serial.printf("GroupHeadStateHandler %d: Finished Extraction - Current: %ld pulses; Target: %ld pulses; Difference: %ld pulses\n", groupNumber, currentPulses, targetPulses, currentPulses - targetPulses);

            targetPulses = 0;
        }
    }
    else if (*event != GroupHeadButtonEvent::NONE && *event != GroupHeadButtonEvent::CONTINUOUS_HELD && *event != GroupHeadButtonEvent::AUTO_BACKFLUSH)
    {
        targetPulses = 0;

        if (*event == GroupHeadButtonEvent::CONTINUOUS)
        {
            targetPulses = LONG_MAX;
        }
        else
        {
            targetPulses = volumetricsHelper->getFlowMeterSetting(*event);

            buttonToBeProgrammed = *event;
        }

        if (targetPulses != 0 || (*event != GroupHeadButtonEvent::CONTINUOUS && *isInProgrammingMode))
        {
            currentPulses = 0;

            *isExtracting = true;
            Serial.printf("GroupHeadStateHandler %d: Start extracting targeting %ld pulses\n", groupNumber, targetPulses);
        }
    }
}

void GroupHeadStateHandler::flowMeterPulseInterrupt()
{
    if (*isExtracting)
    {
        currentPulses++;
    }
}

void GroupHeadStateHandler::startAutoBackflush()
{
    Serial.printf("GroupHeadStateHandler %d: Starting auto backflush sequence\n", groupNumber);
    isAutoBackflushing = true;
    autoBackflushCycle = 0;
    autoBackflushStartTime = millis();
    *isExtracting = true; // Start with extraction
}

void GroupHeadStateHandler::handleAutoBackflush()
{
    unsigned long elapsed = millis() - autoBackflushStartTime;
    int cyclePosition = autoBackflushCycle % 2; // 0 = extract, 1 = pause

    int currentCycle = (autoBackflushCycle / 2) + 1;
    if (cyclePosition == 0) // Extract phase
    {
        if (elapsed >= EXTRACT_DURATION_MS)
        {
            if (currentCycle >= TOTAL_CYCLES)
            {
                Serial.printf("GroupHeadStateHandler %d: Auto backflush complete after %d cycles\n",
                              groupNumber, currentCycle);
                stopAutoBackflush();
            }
            else
            {
                Serial.printf("GroupHeadStateHandler %d: Extract phase %d complete, starting pause\n",
                              groupNumber, currentCycle);
                *isExtracting = false;
                autoBackflushCycle++;
                autoBackflushStartTime = millis();
            }
        }
    }
    else // Pause phase
    {
        if (elapsed >= PAUSE_DURATION_MS)
        {
            Serial.printf("GroupHeadStateHandler %d: Pause phase complete, starting extract phase %d\n",
                          groupNumber, currentCycle + 1);
            *isExtracting = true;
            autoBackflushCycle++;
            autoBackflushStartTime = millis();
        }
    }
}

void GroupHeadStateHandler::stopAutoBackflush()
{
    isAutoBackflushing = false;
    autoBackflushCycle = 0;
    *isExtracting = false;
}
