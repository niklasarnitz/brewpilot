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

public:
    GroupHeadStateHandler(bool *isExtracting, GroupHeadButtonEvent *event, VolumetricsHelper *volumetricsHelper, PreferenceHelper *preferenceHelper, bool *isInProgrammingMode, int groupNumber);
    void handleState() override;
    void flowMeterPulseInterrupt();

    // Reload auto-backflush settings from preferences
    void reloadAutoBackflushSettings();

    // BLE access to progress data
    unsigned long getCurrentPulses() const { return currentPulses; }
    unsigned long getTargetPulses() const { return targetPulses; }

private:
    void startAutoBackflush();
    void handleAutoBackflush();
    void stopAutoBackflush();
};

#endif // BREWPILOT_GROUPHEADHANDLER_H
