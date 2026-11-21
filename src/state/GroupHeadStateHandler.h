#pragma once

#include <atomic>
#include <stdint.h>

#include "input/ButtonEvents.h"
#include "state/GenericStateHandler.h"
#include "storage/VolumetricsHelper.h"

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
    GroupHeadStateHandler(bool *isExtracting, GroupHeadButtonEvent *event, VolumetricsHelper *volumetricsHelper, ProgrammingTarget *programmingTarget, int groupNumber);

    void handleState() override;

    // Runs in interrupt context and is placed in IRAM.
    void flowMeterPulseInterrupt();

    void stop();

private:
    void startExtraction(uint32_t totalPulses);

    void startAutoBackflush();

    void handleAutoBackflush();

    void stopAutoBackflush();
};
