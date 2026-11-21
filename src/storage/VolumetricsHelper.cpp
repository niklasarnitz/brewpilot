#include "storage/VolumetricsHelper.h"

#include <Arduino.h>
#include <inttypes.h>

void VolumetricsHelper::logVolumetricSettings() const
{
    Serial.println("Volumetric Settings:");
    Serial.printf("Left Single Espresso: %" PRIu32 " pulses\n", volumetricSettings.leftSingleEspressoPulses);
    Serial.printf("Left Double Espresso: %" PRIu32 " pulses\n", volumetricSettings.leftDoubleEspressoPulses);
    Serial.printf("Right Single Espresso: %" PRIu32 " pulses\n", volumetricSettings.rightSingleEspressoPulses);
    Serial.printf("Right Double Espresso: %" PRIu32 " pulses\n", volumetricSettings.rightDoubleEspressoPulses);
    Serial.printf("Tea Water: %" PRIu32 " ms\n", volumetricSettings.teaWaterMilliseconds);
}

PreferenceKey VolumetricsHelper::convertButtonEventToPreferenceKey(GroupHeadButtonEvent event) const
{
    switch (event)
    {
    case GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO:
        return PreferenceKey::FlowMeterPulsesLeftSingleEspresso;
    case GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO:
        return PreferenceKey::FlowMeterPulsesRightSingleEspresso;
    case GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO:
        return PreferenceKey::FlowMeterPulsesLeftDoubleEspresso;
    case GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO:
        return PreferenceKey::FlowMeterPulsesRightDoubleEspresso;
    };

    return PreferenceKey::INVALID;
}

VolumetricsHelper::VolumetricsHelper(PreferenceHelper &preferenceHelper)
    : preferenceHelper(preferenceHelper) {}

void VolumetricsHelper::setup()
{
    if (!preferenceHelper.open())
        return;

    volumetricSettings.leftSingleEspressoPulses = preferenceHelper.getULong(PreferenceKey::FlowMeterPulsesLeftSingleEspresso);
    volumetricSettings.leftDoubleEspressoPulses = preferenceHelper.getULong(PreferenceKey::FlowMeterPulsesLeftDoubleEspresso);
    volumetricSettings.rightSingleEspressoPulses = preferenceHelper.getULong(PreferenceKey::FlowMeterPulsesRightSingleEspresso);
    volumetricSettings.rightDoubleEspressoPulses = preferenceHelper.getULong(PreferenceKey::FlowMeterPulsesRightDoubleEspresso);
    volumetricSettings.teaWaterMilliseconds = preferenceHelper.getULong(PreferenceKey::TeaWaterMilliseconds);

    preferenceHelper.close();

    logVolumetricSettings();
}

bool VolumetricsHelper::writeFlowMeterSetting(GroupHeadButtonEvent event, uint32_t value)
{
    if (value == 0 || !preferenceHelper.setULong(convertButtonEventToPreferenceKey(event), value))
        return false;

    switch (event)
    {
    case GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO:
        volumetricSettings.leftSingleEspressoPulses = value;
        break;
    case GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO:
        volumetricSettings.rightSingleEspressoPulses = value;
        break;
    case GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO:
        volumetricSettings.leftDoubleEspressoPulses = value;
        break;
    case GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO:
        volumetricSettings.rightDoubleEspressoPulses = value;
        break;
    };

    logVolumetricSettings();
    return true;
}

bool VolumetricsHelper::writeTeaWaterSetting(uint32_t value)
{
    if (value == 0 || !preferenceHelper.setULong(PreferenceKey::TeaWaterMilliseconds, value))
        return false;

    volumetricSettings.teaWaterMilliseconds = value;

    logVolumetricSettings();
    return true;
}

uint32_t VolumetricsHelper::getFlowMeterSetting(GroupHeadButtonEvent key) const
{
    switch (key)
    {
    case GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO:
        return volumetricSettings.leftSingleEspressoPulses;
    case GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO:
        return volumetricSettings.rightSingleEspressoPulses;
    case GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO:
        return volumetricSettings.leftDoubleEspressoPulses;
    case GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO:
        return volumetricSettings.rightDoubleEspressoPulses;
    };

    // This won't happen - it is just to suppress the compiler warning :D
    return 0;
}

uint32_t VolumetricsHelper::getTeaWaterSetting() const
{
    return volumetricSettings.teaWaterMilliseconds;
}
