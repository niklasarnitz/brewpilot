#include "DeviceNameHelper.h"

DeviceNameHelper::DeviceNameHelper(PreferenceHelper *preferenceHelper)
    : preferenceHelper(preferenceHelper), fullDeviceName(""), deviceNameSuffix("") {}

String DeviceNameHelper::generateRandomSuffix()
{
    // Generate a random 4-digit number
    int randomNum = random(1000, 10000);
    return String(randomNum);
}

void DeviceNameHelper::initialize()
{
    // Check if a suffix is already stored
    preferenceHelper->open();
    deviceNameSuffix = preferenceHelper->getString(PreferenceKey::DeviceNameSuffix, "");
    preferenceHelper->close();

    // If no suffix exists, generate and store a new one
    if (deviceNameSuffix.length() == 0)
    {
        deviceNameSuffix = generateRandomSuffix();
        preferenceHelper->setString(PreferenceKey::DeviceNameSuffix, deviceNameSuffix.c_str());
        Serial.printf("Generated new device name suffix: %s\n", deviceNameSuffix.c_str());
    }
    else
    {
        Serial.printf("Loaded device name suffix: %s\n", deviceNameSuffix.c_str());
    }

    // Build the full device name
    fullDeviceName = "brewpilot-" + deviceNameSuffix;
}

String DeviceNameHelper::getFullDeviceName()
{
    return fullDeviceName;
}

String DeviceNameHelper::getDeviceNameSuffix()
{
    return deviceNameSuffix;
}

void DeviceNameHelper::setDeviceNameSuffix(const char *suffix)
{
    if (suffix != nullptr && strlen(suffix) > 0)
    {
        deviceNameSuffix = String(suffix);
        preferenceHelper->setString(PreferenceKey::DeviceNameSuffix, suffix);

        // Update the full device name
        fullDeviceName = "brewpilot-" + deviceNameSuffix;

        Serial.printf("Updated device name suffix to: %s\n", suffix);
        Serial.printf("Full device name is now: %s\n", fullDeviceName.c_str());
    }
}
