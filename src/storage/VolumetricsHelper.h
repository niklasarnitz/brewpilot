#pragma once

#include <stdint.h>

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

    bool writeFlowMeterSetting(GroupHeadButtonEvent event, uint32_t value);

    bool writeTeaWaterSetting(uint32_t value);

    uint32_t getFlowMeterSetting(GroupHeadButtonEvent key) const;

    uint32_t getTeaWaterSetting() const;
};
