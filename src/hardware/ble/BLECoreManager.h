//
// BLE Core Manager - runs on the second processing core
//

#ifndef BREWPILOT_BLE_CORE_MANAGER_H
#define BREWPILOT_BLE_CORE_MANAGER_H

#include <Arduino.h>
#include "BLEService.h"
#include "hardware/state/State.h"
#include "utils/PreferenceHelper.h"
#include "utils/VolumetricsHelper.h"

class BLECoreManager
{
private:
    BrewPilotBLEService *bleService;
    State *state;
    PreferenceHelper *preferenceHelper;
    VolumetricsHelper *volumetricsHelper;

    // Tracking for state changes
    bool lastIsFillingBoiler = false;
    bool lastGroupOneIsExtracting = false;
    bool lastGroupTwoIsExtracting = false;
    bool lastIsExtractingTeaWater = false;
    bool lastIsInProgrammingMode = false;

    // Last update time for settings refresh
    unsigned long lastSettingsUpdateTime = 0;
    const unsigned long SETTINGS_UPDATE_INTERVAL = 5000; // 5 seconds

public:
    BLECoreManager(State *state, PreferenceHelper *preferenceHelper, VolumetricsHelper *volumetricsHelper);

    ~BLECoreManager();

    void begin(const char *deviceName = "BrewPilot");

    void loop();

private:
    bool hasStateChanged();
};

#endif // BREWPILOT_BLE_CORE_MANAGER_H
