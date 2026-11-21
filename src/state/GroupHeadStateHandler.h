#pragma once

#include <atomic>
#include <stdint.h>

#include "input/ButtonEvents.h"
#include "state/State.h"
#include "storage/VolumetricsHelper.h"

class GroupHeadStateHandler
{
    VolumetricsHelper &volumetricsHelper;
    const GroupHeadButtonEvent &event;
    bool &isExtracting;

    ProgrammingTarget &programmingTarget;
    const ProgrammingTarget ownTarget;

    GroupHeadButtonEvent buttonToBeProgrammed = GroupHeadButtonEvent::NONE;

    uint32_t targetPulses = 0;
    // Set by every extraction start: continuous runs have no pulse target.
    bool isContinuous = false;
    // Only the ISR increments this monotonic counter; the loop never resets it.
    std::atomic<uint32_t> pulseCounter{0};
    uint32_t pulsesAtStart = 0;

    int groupNumber;

    bool isAutoBackflushing = false;
    int autoBackflushCycle = 0;
    uint32_t autoBackflushStartTime = 0;
    static constexpr uint32_t EXTRACT_DURATION_MS = 10000;
    static constexpr uint32_t PAUSE_DURATION_MS = 5000;
    static constexpr int TOTAL_CYCLES = 5;

public:
    GroupHeadStateHandler(bool &isExtracting, const GroupHeadButtonEvent &event, VolumetricsHelper &volumetricsHelper,
                          ProgrammingTarget &programmingTarget, int groupNumber);

    void handleState();

    // Runs in interrupt context and is placed in IRAM.
    void flowMeterPulseInterrupt();

    void stop();

private:
    void startExtraction(uint32_t totalPulses);

    void startAutoBackflush();

    void handleAutoBackflush();

    void stopAutoBackflush();
};
