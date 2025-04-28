//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_GROUPHEADHANDLER_H
#define BREWPILOT_GROUPHEADHANDLER_H

#include "GenericStateHandler.h"
#include "hardware/enums/GroupHeadButtonEvent.h"
#include "utils/VolumetricsHelper.h"

#include <climits>

extern void logMessage(const char *message);

class GroupHeadStateHandler : public GenericStateHandler
{
    VolumetricsHelper *volumetricsHelper;
    GroupHeadButtonEvent *event;
    bool *isExtracting;

    bool *isInProgrammingMode;

    GroupHeadButtonEvent buttonToBeProgrammed = GroupHeadButtonEvent::NONE;

    unsigned long targetPulses = 0;
    unsigned long currentPulses = 0;

public:
    GroupHeadStateHandler(bool *isExtracting, GroupHeadButtonEvent *event, VolumetricsHelper *volumetricsHelper, bool *isInProgrammingMode)
        : volumetricsHelper(volumetricsHelper), event(event), isExtracting(isExtracting), isInProgrammingMode(isInProgrammingMode) {}

    void handleState() override
    {
        if (*isExtracting)
        {
            if (*event == GroupHeadButtonEvent::CONTINUOUS_HELD && targetPulses == LONG_MAX && !(*isInProgrammingMode))
            {
                logMessage("GroupHeadStateHandler: Entered Programming Mode");
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

                    logMessage("GroupHeadStateHandler: Left Programming Mode");
                    *isInProgrammingMode = false;
                }

                currentPulses = 0;
                targetPulses = 0;
            }
            else if (!(*isInProgrammingMode) && currentPulses >= targetPulses)
            {
                *isExtracting = false;

                char buffer[128];
                sprintf(buffer, "GroupHeadStateHandler: Finished Extraction - Current: %ld pulses; Target: %ld pulses; Difference: %ld pulses",
                        currentPulses, targetPulses, currentPulses - targetPulses);
                logMessage(buffer);

                targetPulses = 0;
            }
        }
        else if (*event != GroupHeadButtonEvent::NONE && *event != GroupHeadButtonEvent::CONTINUOUS_HELD)
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
            }
        }

        if (*isExtracting && currentPulses != 0)
        {
            char buffer[128];
            sprintf(buffer, "GroupHeadStateHandler: Current: %ld pulses; Target: %ld pulses; Difference: %ld pulses",
                    currentPulses, targetPulses, currentPulses - targetPulses);
            logMessage(buffer);
        }
    }

    void flowMeterPulseInterrupt()
    {
        if (*isExtracting)
        {
            currentPulses++;
        }
    }
};

#endif // BREWPILOT_GROUPHEADHANDLER_H
