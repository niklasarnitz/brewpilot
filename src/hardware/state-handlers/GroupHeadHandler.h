//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_GROUPHEADHANDLER_H
#define BREWPILOT_GROUPHEADHANDLER_H

#include "GenericStateHandler.h"
#include "hardware/enums/GroupHeadButtonEvent.h"
#include "utils/VolumetricsHelper.h"

#include <atomic>
#include <climits>

class GroupHeadStateHandler : public GenericStateHandler
{
    VolumetricsHelper *volumetricsHelper;
    GroupHeadButtonEvent *event;
    bool *isExtracting;

    ProgrammingTarget *programmingTarget;
    const ProgrammingTarget ownTarget;

    GroupHeadButtonEvent buttonToBeProgrammed = GroupHeadButtonEvent::NONE;

    unsigned long targetPulses = 0;
    // Only the ISR increments this monotonic counter; the loop never resets it.
    std::atomic<uint32_t> pulseCounter{0};
    uint32_t pulsesAtStart = 0;

    int groupNumber;

    bool isAutoBackflushing = false;
    int autoBackflushCycle = 0;
    uint32_t autoBackflushStartTime = 0;
    static const int EXTRACT_DURATION_MS = 10000; // 10 seconds
    static const int PAUSE_DURATION_MS = 5000;    // 5 seconds
    static const int TOTAL_CYCLES = 5;

public:
    GroupHeadStateHandler(bool *isExtracting, GroupHeadButtonEvent *event, VolumetricsHelper *volumetricsHelper, ProgrammingTarget *programmingTarget, int groupNumber)
        : volumetricsHelper(volumetricsHelper), event(event), isExtracting(isExtracting), programmingTarget(programmingTarget),
          ownTarget(groupNumber == 1 ? ProgrammingTarget::GROUP_ONE : ProgrammingTarget::GROUP_TWO), groupNumber(groupNumber) {}

    void handleState() override
    {
        if (*event == GroupHeadButtonEvent::CONTINUOUS && *programmingTarget == ownTarget &&
            !*isExtracting)
        {
            stop();
            Serial.println("GroupHeadStateHandler: Programming selection canceled");
            return;
        }

        // Handle auto backflush mode
        if (isAutoBackflushing)
        {
            handleAutoBackflush();
            return; // Auto backflush takes priority over normal operations
        }

        const uint32_t totalPulses = pulseCounter.load(std::memory_order_relaxed);
        const unsigned long currentPulses = uint32_t(totalPulses - pulsesAtStart);

        // Check for auto backflush activation
        if (*event == GroupHeadButtonEvent::AUTO_BACKFLUSH && *programmingTarget == ProgrammingTarget::NONE)
        {
            startAutoBackflush();
            return;
        }

        // Normal extraction handling
        if (*isExtracting)
        {
            if (*event == GroupHeadButtonEvent::CONTINUOUS_HELD && targetPulses == LONG_MAX && *programmingTarget == ProgrammingTarget::NONE)
            {
                Serial.printf("GroupHeadStateHandler %d: Entered Programming Mode\n", groupNumber);
                stop();
                *programmingTarget = ownTarget;
            }
            else if (*event == GroupHeadButtonEvent::CONTINUOUS)
            {
                *isExtracting = false;

                if (*programmingTarget == ownTarget)
                {
                    bool saved = volumetricsHelper->writeFlowMeterSetting(buttonToBeProgrammed, currentPulses);
                    Serial.printf("GroupHeadStateHandler %d: %s. Left Programming Mode\n", groupNumber,
                                  saved ? "Dose saved" : "Dose NOT saved; previous setting retained");
                }
                else
                {
                    Serial.printf("GroupHeadStateHandler %d: Cancelled Extraction at %ld of %ld pulses\n", groupNumber, currentPulses, targetPulses);
                }

                stop();
            }
            else if (*programmingTarget != ownTarget && currentPulses >= targetPulses)
            {
                *isExtracting = false;

                Serial.printf("GroupHeadStateHandler %d: Finished Extraction - Current: %ld pulses; Target: %ld pulses; Difference: %ld pulses\n", groupNumber, currentPulses, targetPulses, currentPulses - targetPulses);

                stop();
            }
        }
        else if (*event != GroupHeadButtonEvent::NONE && *event != GroupHeadButtonEvent::CONTINUOUS_HELD && *event != GroupHeadButtonEvent::AUTO_BACKFLUSH)
        {
            startExtraction(totalPulses);
        }
    }

    void flowMeterPulseInterrupt()
    {
        pulseCounter.fetch_add(1, std::memory_order_relaxed);
    }

    void stop()
    {
        stopAutoBackflush();
        buttonToBeProgrammed = GroupHeadButtonEvent::NONE;
        if (*programmingTarget == ownTarget)
            *programmingTarget = ProgrammingTarget::NONE;
    }

private:
    void startExtraction(uint32_t totalPulses)
    {
        targetPulses = 0;

        if (*event == GroupHeadButtonEvent::CONTINUOUS)
        {
            targetPulses = LONG_MAX;
        }
        else
        {
            targetPulses = volumetricsHelper->getFlowMeterSetting(*event);

            if (*programmingTarget == ownTarget)
            {
                buttonToBeProgrammed = *event;
            }
        }

        if (targetPulses != 0 || *programmingTarget == ownTarget)
        {
            pulsesAtStart = totalPulses;

            *isExtracting = true;
            Serial.printf("GroupHeadStateHandler %d: Start extracting targeting %ld pulses\n", groupNumber, targetPulses);
        }
    }

    void startAutoBackflush()
    {
        Serial.printf("GroupHeadStateHandler %d: Starting auto backflush sequence\n", groupNumber);
        isAutoBackflushing = true;
        autoBackflushCycle = 0;
        autoBackflushStartTime = millis();
        *isExtracting = true; // Start with extraction
        targetPulses = 0;
    }

    void handleAutoBackflush()
    {
        if (*event == GroupHeadButtonEvent::CONTINUOUS)
        {
            Serial.printf("GroupHeadStateHandler %d: Auto backflush canceled\n", groupNumber);
            stopAutoBackflush();
            return;
        }

        const auto elapsed = uint32_t(millis() - autoBackflushStartTime);
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

    void stopAutoBackflush()
    {
        isAutoBackflushing = false;
        autoBackflushCycle = 0;
        *isExtracting = false;
        targetPulses = 0;
    }
};

#endif // BREWPILOT_GROUPHEADHANDLER_H
