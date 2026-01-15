//
// Created by Niklas Arnitz on 11.11.24.
//

#include "PreferenceHelper.h"

const char *PreferenceHelper::toString(PreferenceKey key) const
{
    switch (key)
    {
    case PreferenceKey::FlowMeterPulsesLeftSingleEspresso:
        return "FM_P_L_S";
    case PreferenceKey::FlowMeterPulsesRightSingleEspresso:
        return "FM_P_R_S";
    case PreferenceKey::FlowMeterPulsesLeftDoubleEspresso:
        return "FM_P_L_D";
    case PreferenceKey::FlowMeterPulsesRightDoubleEspresso:
        return "FM_P_R_D";
    case PreferenceKey::TeaWaterMilliseconds:
        return "T_M";
    case PreferenceKey::BackflushActivationTimeMs:
        return "BF_ACT_MS";
    case PreferenceKey::BackflushDeactivationTimeMs:
        return "BF_DEACT_MS";
    case PreferenceKey::AutoBackflushExtractDurationMs:
        return "ABF_EXT_MS";
    case PreferenceKey::AutoBackflushPauseDurationMs:
        return "ABF_PSE_MS";
    case PreferenceKey::AutoBackflushCycles:
        return "ABF_CYCLES";
    case PreferenceKey::DeviceNameSuffix:
        return "DEV_NAME";
    }

    // This won't happen - it is just to suppress the compiler warning :D
    return "INVALID";
}

void PreferenceHelper::setULong(PreferenceKey key, unsigned long value)
{
    if (key == PreferenceKey::INVALID)
    {
        Serial.println("PreferenceHelper::setULong::Invalid_Key");
        return;
    }

    open();
    preferences.putULong(toString(key), value);
    close();
}

unsigned long PreferenceHelper::getULong(PreferenceKey key, unsigned long defaultValue)
{
    if (key == PreferenceKey::INVALID)
    {
        Serial.println("PreferenceHelper::getULong::Invalid_Key");
        return defaultValue;
    }

    unsigned long returnValue = preferences.getULong(toString(key), defaultValue);

    return returnValue;
}

void PreferenceHelper::setString(PreferenceKey key, const char *value)
{
    if (key == PreferenceKey::INVALID)
    {
        Serial.println("PreferenceHelper::setString::Invalid_Key");
        return;
    }

    open();
    preferences.putString(toString(key), value);
    close();
}

String PreferenceHelper::getString(PreferenceKey key, const char *defaultValue)
{
    if (key == PreferenceKey::INVALID)
    {
        Serial.println("PreferenceHelper::getString::Invalid_Key");
        return String(defaultValue);
    }

    String returnValue = preferences.getString(toString(key), defaultValue);

    return returnValue;
}

void PreferenceHelper::open()
{
    preferences.begin("brewpilot", false);
}

void PreferenceHelper::close()
{
    preferences.end();
}
