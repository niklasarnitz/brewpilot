//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_GROUPHEADHANDLER_H
#define BREWPILOT_GROUPHEADHANDLER_H

#include "GenericStateHandler.h"
#include "hardware/enums/GroupHeadButtonEvent.h"
#include "utils/VolumetricsHelper.h"

// Auto-backflush default values (used as presets for preferences)
constexpr unsigned long DEFAULT_EXTRACT_DURATION_MS = 10000; // 10 seconds
constexpr unsigned long DEFAULT_PAUSE_DURATION_MS = 5000;    // 5 seconds
constexpr unsigned long DEFAULT_TOTAL_CYCLES = 5;

// Backflush default values
constexpr unsigned long DEFAULT_BACKFLUSH_ACTIVATION_TIME_MS = 10000;  // 10000 ms
constexpr unsigned long DEFAULT_BACKFLUSH_DEACTIVATION_TIME_MS = 5000; // 5000 ms

class GroupHeadStateHandler : public GenericStateHandler
{
    VolumetricsHelper *volumetricsHelper;
    PreferenceHelper *preferenceHelper;
    GroupHeadButtonEvent *event;
    bool *isExtracting;
    bool *isInProgrammingMode;
    GroupHeadButtonEvent buttonToBeProgrammed = GroupHeadButtonEvent::NONE;
    unsigned long targetPulses = 0;
    unsigned long currentPulses = 0;
    int groupNumber;
    bool isAutoBackflushing = false;
    int autoBackflushCycle = 0;
    unsigned long autoBackflushStartTime = 0;

    // Cached auto-backflush settings (loaded at startup and when changed)
    unsigned long cachedExtractDuration = DEFAULT_EXTRACT_DURATION_MS;
    unsigned long cachedPauseDuration = DEFAULT_PAUSE_DURATION_MS;
    unsigned long cachedTotalCycles = DEFAULT_TOTAL_CYCLES;

    // Cached backflush settings (loaded at startup and when changed)
    unsigned long cachedBackflushActivationTime = DEFAULT_BACKFLUSH_ACTIVATION_TIME_MS;
    unsigned long cachedBackflushDeactivationTime = DEFAULT_BACKFLUSH_DEACTIVATION_TIME_MS;

public:
    GroupHeadStateHandler(bool *isExtracting, GroupHeadButtonEvent *event, VolumetricsHelper *volumetricsHelper, PreferenceHelper *preferenceHelper, bool *isInProgrammingMode, int groupNumber);
    void handleState() override;
    void flowMeterPulseInterrupt();

    // Reload settings from preferences
    void reloadAutoBackflushSettings();
    void reloadBackflushSettings();

    // BLE access to progress data
    unsigned long getCurrentPulses() const { return currentPulses; }
    unsigned long getTargetPulses() const { return targetPulses; }

    // BLE access to backflush settings
    unsigned long getBackflushActivationTime() const { return cachedBackflushActivationTime; }
    unsigned long getBackflushDeactivationTime() const { return cachedBackflushDeactivationTime; }

private:
    void startAutoBackflush();
    void handleAutoBackflush();
    void stopAutoBackflush();
};

#endif // BREWPILOT_GROUPHEADHANDLER_H
