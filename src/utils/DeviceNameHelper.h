#ifndef BREWPILOT_DEVICENAMEHELPER_H
#define BREWPILOT_DEVICENAMEHELPER_H

#include <Arduino.h>
#include "PreferenceHelper.h"

class DeviceNameHelper
{
private:
    PreferenceHelper *preferenceHelper;
    String fullDeviceName;
    String deviceNameSuffix;

    String generateRandomSuffix();

public:
    DeviceNameHelper(PreferenceHelper *preferenceHelper);

    void initialize();

    String getFullDeviceName();

    String getDeviceNameSuffix();

    void setDeviceNameSuffix(const char *suffix);
};

#endif
