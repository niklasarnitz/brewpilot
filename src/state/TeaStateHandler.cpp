#include "state/TeaStateHandler.h"

#include <Arduino.h>

TeaStateHandler::TeaStateHandler(ButtonEventType *event, bool *isExtractingTeaWater, VolumetricsHelper *volumetricsHelper, ProgrammingTarget *programmingTarget)
    : event(event),
      isExtractingTeaWater(isExtractingTeaWater),
      programmingTarget(programmingTarget),
      volumetricsHelper(volumetricsHelper) {}

void TeaStateHandler::handleState()
{
    if (*event == ButtonEventType::BUTTON_HELD && *programmingTarget == ProgrammingTarget::NONE)
    {
        *isExtractingTeaWater = false;
        *programmingTarget = ProgrammingTarget::TEA;
        Serial.println("TeaStateHandler: Entered programming; press Tea to start learning");
        return;
    }

    if (*isExtractingTeaWater)
    {
        const auto elapsed = uint32_t(millis() - startTime);
        if (*event == ButtonEventType::BUTTON_PRESSED)
        {
            if (*programmingTarget == ProgrammingTarget::TEA)
            {
                bool saved = volumetricsHelper->writeTeaWaterSetting(elapsed);
                Serial.println(saved ? "TeaStateHandler: Tea duration saved; left programming"
                                     : "TeaStateHandler: Tea duration NOT saved; left programming");
                *programmingTarget = ProgrammingTarget::NONE;
            }

            *isExtractingTeaWater = false;
        }

        if (*programmingTarget != ProgrammingTarget::TEA && elapsed >= volumetricsHelper->getTeaWaterSetting())
        {
            Serial.println("TeaStateHandler: Stop extracting");
            *isExtractingTeaWater = false;
        }
    }
    else if ((volumetricsHelper->getTeaWaterSetting() != 0 || *programmingTarget == ProgrammingTarget::TEA) &&
             *event == ButtonEventType::BUTTON_PRESSED)
    {
        Serial.println("TeaStateHandler: Start extracting");
        *isExtractingTeaWater = true;
        startTime = millis();
    }
}
