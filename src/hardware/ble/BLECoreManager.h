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
    BLECoreManager(State *state, PreferenceHelper *preferenceHelper, VolumetricsHelper *volumetricsHelper)
        : state(state), preferenceHelper(preferenceHelper), volumetricsHelper(volumetricsHelper), bleService(nullptr) {}

    ~BLECoreManager()
    {
        if (bleService != nullptr)
        {
            delete bleService;
            bleService = nullptr;
        }
    }

    void begin(const char *deviceName = "BrewPilot")
    {
        bleService = new BrewPilotBLEService(state, preferenceHelper, volumetricsHelper);
        bleService->begin(deviceName);

        // Load and send initial backflush settings (same for both groups initially)
        uint16_t groupOneBackflush = (uint16_t)preferenceHelper->getULong(
            PreferenceKey::BackflushActivationTimeMs, 500);
        uint16_t groupTwoBackflush = (uint16_t)preferenceHelper->getULong(
            PreferenceKey::BackflushDeactivationTimeMs, 500);

        bleService->updateBackflushSettings(groupOneBackflush, groupTwoBackflush);
        bleService->updateVolumetricSettings();

        Serial.printf("BLE: Initialized - Group 1 backflush: %u ms, Group 2 backflush: %u ms\n",
                      groupOneBackflush, groupTwoBackflush);
    }

    void loop()
    {
        if (bleService == nullptr)
        {
            return;
        }

        // Check if state has changed
        if (hasStateChanged())
        {
            bleService->updateState();
        }

        // Periodically refresh settings
        unsigned long currentTime = millis();
        if (currentTime - lastSettingsUpdateTime > SETTINGS_UPDATE_INTERVAL)
        {
            uint16_t groupOneBackflush = (uint16_t)preferenceHelper->getULong(
                PreferenceKey::BackflushActivationTimeMs, 500);
            uint16_t groupTwoBackflush = (uint16_t)preferenceHelper->getULong(
                PreferenceKey::BackflushDeactivationTimeMs, 500);

            bleService->updateBackflushSettings(groupOneBackflush, groupTwoBackflush);
            bleService->updateVolumetricSettings();
            lastSettingsUpdateTime = currentTime;
        }
    }

private:
    bool hasStateChanged()
    {
        bool stateChanged =
            lastIsFillingBoiler != state->isFillingBoiler ||
            lastGroupOneIsExtracting != state->groupOneIsExtracting ||
            lastGroupTwoIsExtracting != state->groupTwoIsExtracting ||
            lastIsExtractingTeaWater != state->isExtractingTeaWater ||
            lastIsInProgrammingMode != state->isInProgrammingMode;

        if (stateChanged)
        {
            // Update tracking variables
            lastIsFillingBoiler = state->isFillingBoiler;
            lastGroupOneIsExtracting = state->groupOneIsExtracting;
            lastGroupTwoIsExtracting = state->groupTwoIsExtracting;
            lastIsExtractingTeaWater = state->isExtractingTeaWater;
            lastIsInProgrammingMode = state->isInProgrammingMode;
        }

        return stateChanged;
    }
};

#endif // BREWPILOT_BLE_CORE_MANAGER_H
