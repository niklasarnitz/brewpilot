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

    unsigned long startTime;

public:
    TeaStateHandler(bool *teaButtonPressed, bool *isExtractingTeaWater, VolumetricsHelper *volumetricsHelper, bool *isInProgrammingMode)
        : teaButtonPressed(teaButtonPressed),
          isExtractingTeaWater(isExtractingTeaWater),
          volumetricsHelper(volumetricsHelper),
          isInProgrammingMode(isInProgrammingMode)
    {
        startTime = 0;
    }

    void handleState() override
    {
        if (*isExtractingTeaWater)
        {
            if (*teaButtonPressed)
            {
                if (*isInProgrammingMode)
                {
                    volumetricsHelper->writeTeaWaterSetting(millis() - startTime);

                    Serial.println("Deactivating Programming Mode because of Tea");
                    *isInProgrammingMode = false;
                    *isExtractingTeaWater = false;
                }
            }

            if (!(*isInProgrammingMode) && ((millis() - startTime) >= volumetricsHelper->getTeaWaterSetting()))
            {
                Serial.println("Tea water stops extracting");
                *isExtractingTeaWater = false;
            }
        }
        else if ((volumetricsHelper->getTeaWaterSetting() != 0 || *isInProgrammingMode) && *teaButtonPressed && !*isExtractingTeaWater)
        {
            Serial.println("Tea water starts extracting");
            *isExtractingTeaWater = true;
            startTime = millis();
        }
    }
};

#endif // BREWPILOT_TEASTATEHANDLER_H
