//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_TEASTATEHANDLER_H
#define BREWPILOT_TEASTATEHANDLER_H

#include <Arduino.h>

#include "GenericStateHandler.h"
#include "../../utils/VolumetricsHelper.h"

class TeaStateHandler : public GenericStateHandler
{
private:
    bool *teaButtonPressed;
    bool *isExtractingTeaWater;
    bool *isInProgrammingMode;
    VolumetricsHelper *volumetricsHelper;
    unsigned long startTime = 0;

public:
    TeaStateHandler(bool *teaButtonPressed, bool *isExtractingTeaWater, VolumetricsHelper *volumetricsHelper, bool *isInProgrammingMode);
    void handleState() override;
};

#endif // BREWPILOT_TEASTATEHANDLER_H
