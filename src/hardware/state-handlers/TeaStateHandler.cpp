//
// Created by Niklas Arnitz on 17.11.24.
//

#include "TeaStateHandler.h"

TeaStateHandler::TeaStateHandler(bool *teaButtonPressed, bool *isExtractingTeaWater, VolumetricsHelper *volumetricsHelper, bool *isInProgrammingMode)
    : teaButtonPressed(teaButtonPressed),
      isExtractingTeaWater(isExtractingTeaWater),
      isInProgrammingMode(isInProgrammingMode),
      volumetricsHelper(volumetricsHelper)
{
}

void TeaStateHandler::handleState()
{
    if (*isExtractingTeaWater)
    {
        if (*teaButtonPressed)
        {
            if (*isInProgrammingMode)
            {
                volumetricsHelper->writeTeaWaterSetting(millis() - startTime);

                Serial.println("TeaStateHandler: Stored tea extraction time. Left Programming Mode");
                *isInProgrammingMode = false;
            }

            *isExtractingTeaWater = false;
        }

        if (!(*isInProgrammingMode) && ((millis() - startTime) >= volumetricsHelper->getTeaWaterSetting()))
        {
            Serial.println("TeaStateHandler: Stop extracting");
            *isExtractingTeaWater = false;
        }
    }
    else if ((volumetricsHelper->getTeaWaterSetting() != 0 || *isInProgrammingMode) && *teaButtonPressed && !*isExtractingTeaWater)
    {
        Serial.println("TeaStateHandler: Start extracting");
        *isExtractingTeaWater = true;
        startTime = millis();
    }
}
