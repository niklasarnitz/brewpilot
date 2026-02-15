//
// Created by Niklas Arnitz on 16.11.24.
//

#include "GroupHeadHandler.h"
#include <climits>

GroupHeadStateHandler::GroupHeadStateHandler(bool *isExtracting, GroupHeadButtonEvent *event, VolumetricsHelper *volumetricsHelper, PreferenceHelper *preferenceHelper, bool *isInProgrammingMode, int groupNumber)
    : volumetricsHelper(volumetricsHelper), preferenceHelper(preferenceHelper), event(event), isExtracting(isExtracting), isInProgrammingMode(isInProgrammingMode), groupNumber(groupNumber)
{
    // Load settings at startup
    reloadAutoBackflushSettings();
    reloadBackflushSettings();
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
    currentPulses = 0;
    targetPulses = 0;
}

void GroupHeadStateHandler::handleAutoBackflush()
{
    // Use cached auto-backflush settings
    unsigned long elapsed = millis() - autoBackflushStartTime;
    int cyclePosition = autoBackflushCycle % 2; // 0 = extract, 1 = pause

    int currentCycle = (autoBackflushCycle / 2) + 1;
    if (cyclePosition == 0) // Extract phase
    {
        if (elapsed >= cachedExtractDuration)
        {
            if (currentCycle >= cachedTotalCycles)
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
        if (elapsed >= cachedPauseDuration)
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
    currentPulses = 0;
    targetPulses = 0;
}

void GroupHeadStateHandler::reloadAutoBackflushSettings()
{
    cachedExtractDuration = preferenceHelper->getULong(PreferenceKey::AutoBackflushExtractDurationMs, DEFAULT_EXTRACT_DURATION_MS);
    cachedPauseDuration = preferenceHelper->getULong(PreferenceKey::AutoBackflushPauseDurationMs, DEFAULT_PAUSE_DURATION_MS);
    cachedTotalCycles = preferenceHelper->getULong(PreferenceKey::AutoBackflushCycles, DEFAULT_TOTAL_CYCLES);

    Serial.printf("GroupHeadStateHandler %d: Loaded auto-backflush settings - Extract: %lu ms, Pause: %lu ms, Cycles: %lu\n",
                  groupNumber, cachedExtractDuration, cachedPauseDuration, cachedTotalCycles);
}

void GroupHeadStateHandler::reloadBackflushSettings()
{
    cachedBackflushActivationTime = preferenceHelper->getULong(PreferenceKey::BackflushActivationTimeMs, DEFAULT_BACKFLUSH_ACTIVATION_TIME_MS);
    cachedBackflushDeactivationTime = preferenceHelper->getULong(PreferenceKey::BackflushDeactivationTimeMs, DEFAULT_BACKFLUSH_DEACTIVATION_TIME_MS);

    Serial.printf("GroupHeadStateHandler %d: Loaded backflush settings - Activation: %lu ms, Deactivation: %lu ms\n",
                  groupNumber, cachedBackflushActivationTime, cachedBackflushDeactivationTime);
}
