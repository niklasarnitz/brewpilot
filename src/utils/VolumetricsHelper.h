#ifndef BREWPILOT_VOLUMETRICSHELPER_H
#define BREWPILOT_VOLUMETRICSHELPER_H

#include <Arduino.h>

#include "structs/VolumetricSettings.h"
#include "hardware/enums/GroupHeadButtonEvent.h"
#include "PreferenceHelper.h"

class VolumetricsHelper
{
private:
    VolumetricSettings volumetricSettings{};

    PreferenceHelper *preferenceHelper;

    void logVolumetricSettings(VolumetricSettings *volumetricSettings)
    {
        Serial.printf("Volumetric Settings:\n");
        Serial.printf("Left Single Espresso: %ld pulses\n", volumetricSettings->leftSingleEspressoPulses);
        Serial.printf("Left Double Espresso: %ld pulses\n", volumetricSettings->leftDoubleEspressoPulses);
        Serial.printf("Right Single Espresso: %ld pulses\n", volumetricSettings->rightSingleEspressoPulses);
        Serial.printf("Right Double Espresso: %ld pulses\n", volumetricSettings->rightDoubleEspressoPulses);
        Serial.printf("Tea Water: %ld ms\n", volumetricSettings->teaWaterMilliseconds);
    }

    PreferenceKey convertButtonEventToPreferenceKey(GroupHeadButtonEvent event)
    {
        switch (event)
        {
        case GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO:
            return PreferenceKey::FlowMeterPulsesLeftSingleEspresso;
        case GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO:
            return PreferenceKey::FlowMeterPulsesRightSingleEspresso;
        case GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO:
            return PreferenceKey::FlowMeterPulsesLeftDoubleEspresso;
        case GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO:
            return PreferenceKey::FlowMeterPulsesRightDoubleEspresso;
        };

        return PreferenceKey::INVALID;
    }

public:
    explicit VolumetricsHelper(PreferenceHelper *preferenceHelper)
        : preferenceHelper(preferenceHelper) {};

    void setup()
    {
        preferenceHelper->open();

        volumetricSettings.leftSingleEspressoPulses = preferenceHelper->getULong(PreferenceKey::FlowMeterPulsesLeftSingleEspresso);
        volumetricSettings.leftDoubleEspressoPulses = preferenceHelper->getULong(PreferenceKey::FlowMeterPulsesLeftDoubleEspresso);
        volumetricSettings.rightSingleEspressoPulses = preferenceHelper->getULong(PreferenceKey::FlowMeterPulsesRightSingleEspresso);
        volumetricSettings.rightDoubleEspressoPulses = preferenceHelper->getULong(PreferenceKey::FlowMeterPulsesRightDoubleEspresso);
        volumetricSettings.teaWaterMilliseconds = preferenceHelper->getULong(PreferenceKey::TeaWaterMilliseconds);

        preferenceHelper->close();

        logVolumetricSettings(&volumetricSettings);
    }

    void writeFlowMeterSetting(GroupHeadButtonEvent event, unsigned long value)
    {
        preferenceHelper->setULong(convertButtonEventToPreferenceKey(event), value);

        switch (event)
        {
        case GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO:
            volumetricSettings.leftSingleEspressoPulses = value;
            break;
        case GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO:
            volumetricSettings.rightSingleEspressoPulses = value;
            break;
        case GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO:
            volumetricSettings.leftDoubleEspressoPulses = value;
            break;
        case GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO:
            volumetricSettings.rightDoubleEspressoPulses = value;
            break;
        };

        logVolumetricSettings(&volumetricSettings);
    }

    void writeTeaWaterSetting(long value)
    {
        preferenceHelper->setULong(PreferenceKey::TeaWaterMilliseconds, value);

        volumetricSettings.teaWaterMilliseconds = value;

        logVolumetricSettings(&volumetricSettings);
    }

    unsigned long getFlowMeterSetting(GroupHeadButtonEvent key)
    {
        switch (key)
        {
        case GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO:
            return volumetricSettings.leftSingleEspressoPulses;
        case GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO:
            return volumetricSettings.rightSingleEspressoPulses;
        case GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO:
            return volumetricSettings.leftDoubleEspressoPulses;
        case GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO:
            return volumetricSettings.rightDoubleEspressoPulses;
        };

        // This won't happen - it is just to suppress the compiler warning :D
        return 0;
    }

    unsigned long getTeaWaterSetting()
    {
        return volumetricSettings.teaWaterMilliseconds;
    }
};

#endif // BREWPILOT_VOLUMETRICSHELPER_H