#include "VolumetricsHelper.h"

void VolumetricsHelper::logVolumetricSettings(VolumetricSettings *settings)
{
    Serial.println("Volumetric Settings:");
    Serial.printf("Left Single Espresso: %ld pulses\n", settings->leftSingleEspressoPulses);
    Serial.printf("Left Double Espresso: %ld pulses\n", settings->leftDoubleEspressoPulses);
    Serial.printf("Right Single Espresso: %ld pulses\n", settings->rightSingleEspressoPulses);
    Serial.printf("Right Double Espresso: %ld pulses\n", settings->rightDoubleEspressoPulses);
    Serial.printf("Tea Water: %ld ms\n", settings->teaWaterMilliseconds);
}

PreferenceKey VolumetricsHelper::convertButtonEventToPreferenceKey(GroupHeadButtonEvent event)
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

VolumetricsHelper::VolumetricsHelper(PreferenceHelper *preferenceHelper)
    : preferenceHelper(preferenceHelper)
{
}

void VolumetricsHelper::setup()
{
    preferenceHelper->open();

    volumetricSettings.leftSingleEspressoPulses = preferenceHelper->getULong(PreferenceKey::FlowMeterPulsesLeftSingleEspresso);
    volumetricSettings.leftDoubleEspressoPulses = preferenceHelper->getULong(PreferenceKey::FlowMeterPulsesLeftDoubleEspresso);
    volumetricSettings.rightSingleEspressoPulses = preferenceHelper->getULong(PreferenceKey::FlowMeterPulsesRightSingleEspresso);
    volumetricSettings.rightDoubleEspressoPulses = preferenceHelper->getULong(PreferenceKey::FlowMeterPulsesRightDoubleEspresso);
    volumetricSettings.teaWaterMilliseconds = preferenceHelper->getULong(PreferenceKey::TeaWaterMilliseconds);

    preferenceHelper->close();

    logVolumetricSettings(&volumetricSettings);
}

void VolumetricsHelper::writeFlowMeterSetting(GroupHeadButtonEvent event, unsigned long value)
{
    preferenceHelper->setULong(convertButtonEventToPreferenceKey(event), value);

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

    logVolumetricSettings(&volumetricSettings);
}

void VolumetricsHelper::writeTeaWaterSetting(long value)
{
    preferenceHelper->setULong(PreferenceKey::TeaWaterMilliseconds, value);

    volumetricSettings.teaWaterMilliseconds = value;

    logVolumetricSettings(&volumetricSettings);
}

unsigned long VolumetricsHelper::getFlowMeterSetting(GroupHeadButtonEvent key)
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

unsigned long VolumetricsHelper::getTeaWaterSetting()
{
    return volumetricSettings.teaWaterMilliseconds;
}
