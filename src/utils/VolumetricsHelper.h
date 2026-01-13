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

    void logVolumetricSettings(VolumetricSettings *volumetricSettings);

    PreferenceKey convertButtonEventToPreferenceKey(GroupHeadButtonEvent event);

public:
    explicit VolumetricsHelper(PreferenceHelper *preferenceHelper);

    void setup();

    void writeFlowMeterSetting(GroupHeadButtonEvent event, unsigned long value);

    void writeTeaWaterSetting(long value);

    unsigned long getFlowMeterSetting(GroupHeadButtonEvent key);

    unsigned long getTeaWaterSetting();
};

#endif // BREWPILOT_VOLUMETRICSHELPER_H