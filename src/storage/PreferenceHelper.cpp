#include "storage/PreferenceHelper.h"

#include <Arduino.h>

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
    }

    // This won't happen - it is just to suppress the compiler warning :D
    return "INVALID";
}

bool PreferenceHelper::setULong(PreferenceKey key, uint32_t value)
{
    if (key == PreferenceKey::INVALID)
    {
        Serial.println("PreferenceHelper::setULong::Invalid_Key");
        return false;
    }

    if (!open())
        return false;
    bool saved = preferences.putULong(toString(key), value) == sizeof(uint32_t);
    close();
    if (!saved)
        Serial.println("PreferenceHelper: NVS write failed; previous setting retained");
    return saved;
}

uint32_t PreferenceHelper::getULong(PreferenceKey key, uint32_t defaultValue)
{
    if (key == PreferenceKey::INVALID)
    {
        Serial.println("PreferenceHelper::getULong::Invalid_Key");
        return defaultValue;
    }

    uint32_t returnValue = preferences.getULong(toString(key), defaultValue);

    return returnValue;
}

bool PreferenceHelper::open()
{
    if (!preferences.begin("brewpilot", false))
    {
        Serial.println("PreferenceHelper: Could not open NVS");
        return false;
    }
    return true;
}

void PreferenceHelper::close()
{
    preferences.end();
}
