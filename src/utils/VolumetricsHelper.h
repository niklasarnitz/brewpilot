#ifndef BREWPILOT_VOLUMETRICSHELPER_H
#define BREWPILOT_VOLUMETRICSHELPER_H

#include <Arduino.h>

#include "../structs/VolumetricSettings.h"
#include "../hardware/enums/GroupHeadButtonEvent.h"
#include "PreferenceHelper.h"

extern void logMessage(const char *message);

class VolumetricsHelper
{
private:
    VolumetricSettings volumetricSettings{};

    PreferenceHelper *preferenceHelper;

    void logVolumetricSettings(VolumetricSettings *volumetricSettings)
    {
        char buffer[512]; // Buffer to build the log message

        logMessage("Volumetric Settings:");

        sprintf(buffer, "Left Single Espresso: %ld pulses", volumetricSettings->leftSingleEspressoPulses);
        logMessage(buffer);

        sprintf(buffer, "Left Double Espresso: %ld pulses", volumetricSettings->leftDoubleEspressoPulses);
        logMessage(buffer);

        sprintf(buffer, "Right Single Espresso: %ld pulses", volumetricSettings->rightSingleEspressoPulses);
        logMessage(buffer);

        sprintf(buffer, "Right Double Espresso: %ld pulses", volumetricSettings->rightDoubleEspressoPulses);
        logMessage(buffer);

        sprintf(buffer, "Tea Water: %ld ms", volumetricSettings->teaWaterMilliseconds);
        logMessage(buffer);
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

    VolumetricSettings *getVolumetricSettings()
    {
        return &volumetricSettings;
    }
};

#endif // BREWPILOT_VOLUMETRICSHELPER_H