#pragma once

#include <stdint.h>

#include <Preferences.h>

enum class PreferenceKey
{
    INVALID,
    FlowMeterPulsesLeftSingleEspresso,
    FlowMeterPulsesRightSingleEspresso,
    FlowMeterPulsesLeftDoubleEspresso,
    FlowMeterPulsesRightDoubleEspresso,
    TeaWaterMilliseconds,
};

class PreferenceHelper
{
private:
    Preferences preferences;

    const char *toString(PreferenceKey key) const;

public:
    bool setULong(PreferenceKey key, uint32_t value);

    uint32_t getULong(PreferenceKey key, uint32_t defaultValue = 0);

    bool open();

    void close();
};
