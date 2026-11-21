#include "state/GroupHeadStateHandler.h"

#include <Arduino.h>
#include <inttypes.h>

GroupHeadStateHandler::GroupHeadStateHandler(bool &isExtracting, const GroupHeadButtonEvent &event, VolumetricsHelper &volumetricsHelper,
                                             ProgrammingTarget &programmingTarget, int groupNumber)
    : volumetricsHelper(volumetricsHelper), event(event), isExtracting(isExtracting), programmingTarget(programmingTarget),
      ownTarget(groupNumber == 1 ? ProgrammingTarget::GROUP_ONE : ProgrammingTarget::GROUP_TWO), groupNumber(groupNumber) {}

void GroupHeadStateHandler::handleState()
{
    if (event == GroupHeadButtonEvent::CONTINUOUS && programmingTarget == ownTarget &&
        !isExtracting)
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

    const uint32_t totalPulses = pulseCounter.load();
    const uint32_t currentPulses = totalPulses - pulsesAtStart;

    // Check for auto backflush activation
    if (event == GroupHeadButtonEvent::AUTO_BACKFLUSH && programmingTarget == ProgrammingTarget::NONE)
    {
        startAutoBackflush();
        return;
    }

    // Normal extraction handling
    if (isExtracting)
    {
        if (event == GroupHeadButtonEvent::CONTINUOUS_HELD && isContinuous && programmingTarget == ProgrammingTarget::NONE)
        {
            Serial.printf("GroupHeadStateHandler %d: Entered Programming Mode\n", groupNumber);
            stop();
            programmingTarget = ownTarget;
        }
        else if (event == GroupHeadButtonEvent::CONTINUOUS)
        {
            isExtracting = false;

            if (programmingTarget == ownTarget)
            {
                bool saved = volumetricsHelper.writeFlowMeterSetting(buttonToBeProgrammed, currentPulses);
                Serial.printf("GroupHeadStateHandler %d: %s. Left Programming Mode\n", groupNumber,
                              saved ? "Dose saved" : "Dose NOT saved; previous setting retained");
            }
            else
            {
                if (isContinuous)
                    Serial.printf("GroupHeadStateHandler %d: Stopped continuous extraction at %" PRIu32 " pulses\n", groupNumber, currentPulses);
                else
                    Serial.printf("GroupHeadStateHandler %d: Cancelled Extraction at %" PRIu32 " of %" PRIu32 " pulses\n", groupNumber, currentPulses, targetPulses);
            }

            stop();
        }
        else if (programmingTarget != ownTarget && !isContinuous && currentPulses >= targetPulses)
        {
            isExtracting = false;

            Serial.printf("GroupHeadStateHandler %d: Finished Extraction - Current: %" PRIu32 " pulses; Target: %" PRIu32 " pulses; Difference: %" PRIu32 " pulses\n", groupNumber, currentPulses, targetPulses, currentPulses - targetPulses);

            stop();
        }
    }
    else if (event != GroupHeadButtonEvent::NONE && event != GroupHeadButtonEvent::CONTINUOUS_HELD && event != GroupHeadButtonEvent::AUTO_BACKFLUSH)
    {
        startExtraction(totalPulses);
    }
}

void IRAM_ATTR GroupHeadStateHandler::flowMeterPulseInterrupt()
{
    pulseCounter.fetch_add(1);
}

void GroupHeadStateHandler::stop()
{
    stopAutoBackflush();
    buttonToBeProgrammed = GroupHeadButtonEvent::NONE;
    if (programmingTarget == ownTarget)
        programmingTarget = ProgrammingTarget::NONE;
}

void GroupHeadStateHandler::startExtraction(uint32_t totalPulses)
{
    targetPulses = 0;
    isContinuous = event == GroupHeadButtonEvent::CONTINUOUS;

    if (!isContinuous)
    {
        targetPulses = volumetricsHelper.getFlowMeterSetting(event);

        if (programmingTarget == ownTarget)
        {
            buttonToBeProgrammed = event;
        }
    }

    if (isContinuous || targetPulses != 0 || programmingTarget == ownTarget)
    {
        pulsesAtStart = totalPulses;

        isExtracting = true;
        if (isContinuous)
            Serial.printf("GroupHeadStateHandler %d: Start continuous extraction\n", groupNumber);
        else
            Serial.printf("GroupHeadStateHandler %d: Start extracting targeting %" PRIu32 " pulses\n", groupNumber, targetPulses);
    }
}

void GroupHeadStateHandler::startAutoBackflush()
{
    Serial.printf("GroupHeadStateHandler %d: Starting auto backflush sequence\n", groupNumber);
    isAutoBackflushing = true;
    autoBackflushCycle = 0;
    autoBackflushStartTime = millis();
    isExtracting = true; // Start with extraction
    targetPulses = 0;
}

void GroupHeadStateHandler::handleAutoBackflush()
{
    if (event == GroupHeadButtonEvent::CONTINUOUS)
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
                isExtracting = false;
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
            isExtracting = true;
            autoBackflushCycle++;
            autoBackflushStartTime = millis();
        }
    }
}

void GroupHeadStateHandler::stopAutoBackflush()
{
    isAutoBackflushing = false;
    autoBackflushCycle = 0;
    isExtracting = false;
    targetPulses = 0;
}
