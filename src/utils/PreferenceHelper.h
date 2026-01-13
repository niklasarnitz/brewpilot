#ifndef BREWPILOT_PREFERENCEHELPER_H
#define BREWPILOT_PREFERENCEHELPER_H

#include <Preferences.h>

enum class PreferenceKey
{
    INVALID,
    FlowMeterPulsesLeftSingleEspresso,
    FlowMeterPulsesRightSingleEspresso,
    FlowMeterPulsesLeftDoubleEspresso,
    FlowMeterPulsesRightDoubleEspresso,
    TeaWaterMilliseconds,
    BackflushActivationTimeMs,
    BackflushDeactivationTimeMs,
};

class PreferenceHelper
{
private:
    Preferences preferences;

    const char *toString(PreferenceKey key) const;

public:
    void setULong(PreferenceKey key, unsigned long value);

    unsigned long getULong(PreferenceKey key, unsigned long defaultValue = 0);

    void open();

    void close();
};

#endif