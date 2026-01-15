//
// BLE Core Manager - runs on the second processing core
//

#include "BLECoreManager.h"

BLECoreManager::BLECoreManager(State *state, PreferenceHelper *preferenceHelper, VolumetricsHelper *volumetricsHelper, DeviceNameHelper *deviceNameHelper)
    : state(state), preferenceHelper(preferenceHelper), volumetricsHelper(volumetricsHelper), deviceNameHelper(deviceNameHelper)
{
}

BLECoreManager::~BLECoreManager()
{
    if (bleService != nullptr)
    {
        delete bleService;
        bleService = nullptr;
    }
}

void BLECoreManager::begin()
{
    bleService = new BrewPilotBLEService(state, preferenceHelper, volumetricsHelper, deviceNameHelper);
    bleService->begin(deviceNameHelper->getFullDeviceName().c_str());

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

void BLECoreManager::loop()
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

bool BLECoreManager::hasStateChanged()
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
