#include "storage/VolumetricsHelper.h"

#include <Arduino.h>
#include <inttypes.h>

namespace
{
// Where a dose button's pulse target is kept, in RAM and in NVS.
struct DoseSlot
{
    uint32_t VolumetricSettings::*setting;
    PreferenceKey key;
};

// Only the four dose buttons have a slot; every other event gets {nullptr, INVALID}.
DoseSlot doseSlot(GroupHeadButtonEvent event)
{
    switch (event)
    {
    case GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO:
        return {&VolumetricSettings::leftSingleEspressoPulses, PreferenceKey::FlowMeterPulsesLeftSingleEspresso};
    case GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO:
        return {&VolumetricSettings::rightSingleEspressoPulses, PreferenceKey::FlowMeterPulsesRightSingleEspresso};
    case GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO:
        return {&VolumetricSettings::leftDoubleEspressoPulses, PreferenceKey::FlowMeterPulsesLeftDoubleEspresso};
    case GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO:
        return {&VolumetricSettings::rightDoubleEspressoPulses, PreferenceKey::FlowMeterPulsesRightDoubleEspresso};
    case GroupHeadButtonEvent::CONTINUOUS:
    case GroupHeadButtonEvent::CONTINUOUS_HELD:
    case GroupHeadButtonEvent::AUTO_BACKFLUSH:
    case GroupHeadButtonEvent::NONE:
        break;
    }

    return {nullptr, PreferenceKey::INVALID};
}
} // namespace

void VolumetricsHelper::logVolumetricSettings() const
{
    Serial.println("Volumetric Settings:");
    Serial.printf("Left Single Espresso: %" PRIu32 " pulses\n", volumetricSettings.leftSingleEspressoPulses);
    Serial.printf("Left Double Espresso: %" PRIu32 " pulses\n", volumetricSettings.leftDoubleEspressoPulses);
    Serial.printf("Right Single Espresso: %" PRIu32 " pulses\n", volumetricSettings.rightSingleEspressoPulses);
    Serial.printf("Right Double Espresso: %" PRIu32 " pulses\n", volumetricSettings.rightDoubleEspressoPulses);
    Serial.printf("Tea Water: %" PRIu32 " ms\n", volumetricSettings.teaWaterMilliseconds);
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
    const DoseSlot slot = doseSlot(event);
    if (value == 0 || slot.setting == nullptr || !preferenceHelper.setULong(slot.key, value))
        return false;

    volumetricSettings.*slot.setting = value;

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
    const DoseSlot slot = doseSlot(key);
    return slot.setting == nullptr ? 0 : volumetricSettings.*slot.setting;
}

uint32_t VolumetricsHelper::getTeaWaterSetting() const
{
    return volumetricSettings.teaWaterMilliseconds;
}
