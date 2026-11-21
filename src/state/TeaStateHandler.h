#pragma once

#include <stdint.h>

#include "input/ButtonEvents.h"
#include "state/State.h"
#include "storage/VolumetricsHelper.h"

class TeaStateHandler
{
private:
    ButtonEventType *event;
    bool *isExtractingTeaWater;
    ProgrammingTarget *programmingTarget;

    VolumetricsHelper *volumetricsHelper;

    uint32_t startTime = 0;

public:
    TeaStateHandler(ButtonEventType *event, bool *isExtractingTeaWater, VolumetricsHelper *volumetricsHelper, ProgrammingTarget *programmingTarget);

    void handleState();
};
