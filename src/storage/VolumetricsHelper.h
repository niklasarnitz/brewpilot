#pragma once

#include "input/ButtonEvents.h"
#include "storage/PreferenceHelper.h"
#include "storage/VolumetricSettings.h"

class VolumetricsHelper
{
private:
    VolumetricSettings volumetricSettings{};

    PreferenceHelper *preferenceHelper;

    void logVolumetricSettings(const VolumetricSettings *volumetricSettings) const;

    PreferenceKey convertButtonEventToPreferenceKey(GroupHeadButtonEvent event) const;

public:
    explicit VolumetricsHelper(PreferenceHelper *preferenceHelper);

    void setup();

    bool writeFlowMeterSetting(GroupHeadButtonEvent event, unsigned long value);

    bool writeTeaWaterSetting(unsigned long value);

    unsigned long getFlowMeterSetting(GroupHeadButtonEvent key) const;

    unsigned long getTeaWaterSetting() const;
};
