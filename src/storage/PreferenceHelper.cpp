#include "storage/PreferenceHelper.h"

#include <Arduino.h>

// The on-flash name of a key, or nullptr for PreferenceKey::INVALID.
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
    case PreferenceKey::INVALID:
        break;
    }

    return nullptr;
}

bool PreferenceHelper::setULong(PreferenceKey key, uint32_t value)
{
    const char *name = toString(key);
    if (name == nullptr)
    {
        Serial.println("PreferenceHelper::setULong::Invalid_Key");
        return false;
    }

    if (!open())
        return false;
    bool saved = preferences.putULong(name, value) == sizeof(uint32_t);
    close();
    if (!saved)
        Serial.println("PreferenceHelper: NVS write failed; previous setting retained");
    return saved;
}

uint32_t PreferenceHelper::getULong(PreferenceKey key, uint32_t defaultValue)
{
    const char *name = toString(key);
    if (name == nullptr)
    {
        Serial.println("PreferenceHelper::getULong::Invalid_Key");
        return defaultValue;
    }

    return preferences.getULong(name, defaultValue);
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
