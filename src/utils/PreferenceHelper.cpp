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

void PreferenceHelper::open()
{
    preferences.begin("brewpilot", false);
}

void PreferenceHelper::close()
{
    preferences.end();
}
