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
    ProgrammingTarget *programmingTarget;

    VolumetricsHelper *volumetricsHelper;

    unsigned long startTime = 0;

public:
    TeaStateHandler(bool *teaButtonPressed, bool *isExtractingTeaWater, VolumetricsHelper *volumetricsHelper, ProgrammingTarget *programmingTarget)
        : teaButtonPressed(teaButtonPressed),
          isExtractingTeaWater(isExtractingTeaWater),
          programmingTarget(programmingTarget),
          volumetricsHelper(volumetricsHelper) {}

    void handleState() override
    {
        if (*isExtractingTeaWater)
        {
            if (*teaButtonPressed)
            {
                if (*programmingTarget == ProgrammingTarget::TEA)
                {
                    bool saved = volumetricsHelper->writeTeaWaterSetting(millis() - startTime);
                    Serial.println(saved ? "TeaStateHandler: Tea duration saved; left programming"
                                         : "TeaStateHandler: Tea duration NOT saved; left programming");
                    *programmingTarget = ProgrammingTarget::NONE;
                }

                *isExtractingTeaWater = false;
            }

            if (*programmingTarget != ProgrammingTarget::TEA && ((millis() - startTime) >= volumetricsHelper->getTeaWaterSetting()))
            {
                Serial.println("TeaStateHandler: Stop extracting");
                *isExtractingTeaWater = false;
            }
        }
        else if ((volumetricsHelper->getTeaWaterSetting() != 0 || *programmingTarget == ProgrammingTarget::SELECTING) && *teaButtonPressed)
        {
            if (*programmingTarget == ProgrammingTarget::SELECTING)
                *programmingTarget = ProgrammingTarget::TEA;
            Serial.println("TeaStateHandler: Start extracting");
            *isExtractingTeaWater = true;
            startTime = millis();
        }
    }
};

#endif // BREWPILOT_TEASTATEHANDLER_H
