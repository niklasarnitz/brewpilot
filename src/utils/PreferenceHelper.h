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
    AutoBackflushExtractDurationMs,
    AutoBackflushPauseDurationMs,
    AutoBackflushCycles,
    DeviceNameSuffix,
};

class PreferenceHelper
{
private:
    Preferences preferences;

    const char *toString(PreferenceKey key) const;

public:
    void setULong(PreferenceKey key, unsigned long value);

    unsigned long getULong(PreferenceKey key, unsigned long defaultValue = 0);

    void setString(PreferenceKey key, const char *value);

    String getString(PreferenceKey key, const char *defaultValue = "");

    void open();

    void close();
};

#endif