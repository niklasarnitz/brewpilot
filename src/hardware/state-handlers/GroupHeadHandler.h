//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_GROUPHEADHANDLER_H
#define BREWPILOT_GROUPHEADHANDLER_H

#include "GenericStateHandler.h"
#include "hardware/enums/GroupHeadButtonEvent.h"
#include "utils/VolumetricsHelper.h"

class GroupHeadStateHandler : public GenericStateHandler
{
    VolumetricsHelper *volumetricsHelper;
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
    static const int EXTRACT_DURATION_MS = 10000; // 10 seconds
    static const int PAUSE_DURATION_MS = 5000;    // 5 seconds
    static const int TOTAL_CYCLES = 5;

public:
    GroupHeadStateHandler(bool *isExtracting, GroupHeadButtonEvent *event, VolumetricsHelper *volumetricsHelper, bool *isInProgrammingMode, int groupNumber);
    void handleState() override;
    void flowMeterPulseInterrupt();

private:
    void startAutoBackflush();
    void handleAutoBackflush();
    void stopAutoBackflush();
};

#endif // BREWPILOT_GROUPHEADHANDLER_H
