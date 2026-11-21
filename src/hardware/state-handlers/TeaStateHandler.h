//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_TEASTATEHANDLER_H
#define BREWPILOT_TEASTATEHANDLER_H

#include <Arduino.h>

#include "GenericStateHandler.h"
#include "../handlers/ButtonHandler.h"
#include "../../utils/VolumetricsHelper.h"

class TeaStateHandler : public GenericStateHandler
{
private:
    ButtonEventType *event;
    bool *isExtractingTeaWater;
    ProgrammingTarget *programmingTarget;

    VolumetricsHelper *volumetricsHelper;

    uint32_t startTime = 0;

public:
    TeaStateHandler(ButtonEventType *event, bool *isExtractingTeaWater, VolumetricsHelper *volumetricsHelper, ProgrammingTarget *programmingTarget)
        : event(event),
          isExtractingTeaWater(isExtractingTeaWater),
          programmingTarget(programmingTarget),
          volumetricsHelper(volumetricsHelper) {}

    void handleState() override
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
};

#endif // BREWPILOT_TEASTATEHANDLER_H
