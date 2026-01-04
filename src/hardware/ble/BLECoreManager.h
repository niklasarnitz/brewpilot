//
// BLE Core Manager - runs on the second processing core
//

#ifndef BREWPILOT_BLE_CORE_MANAGER_H
#define BREWPILOT_BLE_CORE_MANAGER_H

#include <Arduino.h>
#include "BLEService.h"
#include "hardware/state/State.h"
#include "utils/PreferenceHelper.h"

class BLECoreManager
{
private:
    BLEService *bleService;
    State *state;
    PreferenceHelper *preferenceHelper;
    
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
    BLECoreManager(State *state, PreferenceHelper *preferenceHelper)
        : state(state), preferenceHelper(preferenceHelper), bleService(nullptr) {}

    void begin(const char *deviceName = "BrewPilot")
    {
        bleService = new BLEService(state, preferenceHelper);
        bleService->begin(deviceName);
        
        // Load and send initial backflush settings
        uint16_t activationTime = (uint16_t)preferenceHelper->getULong(
            PreferenceKey::BackflushActivationTimeMs, 500
        );
        uint16_t deactivationTime = (uint16_t)preferenceHelper->getULong(
            PreferenceKey::BackflushDeactivationTimeMs, 500
        );
        
        bleService->updateBackflushSettings(activationTime, deactivationTime);
        Serial.printf("BLE: Loaded backflush settings - activation: %u ms, deactivation: %u ms\n",
                     activationTime, deactivationTime);
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
        
        // Periodically refresh settings (in case they were updated externally)
        unsigned long currentTime = millis();
        if (currentTime - lastSettingsUpdateTime > SETTINGS_UPDATE_INTERVAL)
        {
            uint16_t activationTime = (uint16_t)preferenceHelper->getULong(
                PreferenceKey::BackflushActivationTimeMs, 500
            );
            uint16_t deactivationTime = (uint16_t)preferenceHelper->getULong(
                PreferenceKey::BackflushDeactivationTimeMs, 500
            );
            
            bleService->updateBackflushSettings(activationTime, deactivationTime);
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
