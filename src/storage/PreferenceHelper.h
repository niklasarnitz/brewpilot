#pragma once

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
    bool setULong(PreferenceKey key, unsigned long value);

    unsigned long getULong(PreferenceKey key, unsigned long defaultValue = 0);

    bool open();

    void close();
};
