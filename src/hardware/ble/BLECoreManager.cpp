//
// BLE Core Manager - runs on the second processing core
//

#include "BLECoreManager.h"

BLECoreManager::BLECoreManager(State *state, StateHandler *stateHandler, PreferenceHelper *preferenceHelper, VolumetricsHelper *volumetricsHelper, DeviceNameHelper *deviceNameHelper)
    : state(state), stateHandler(stateHandler), preferenceHelper(preferenceHelper), volumetricsHelper(volumetricsHelper), deviceNameHelper(deviceNameHelper)
{
}

void BLECoreManager::begin()
{
    bleService = std::make_unique<BrewPilotBLEService>(state, preferenceHelper, volumetricsHelper, deviceNameHelper, stateHandler);
    bleService->begin(deviceNameHelper->getFullDeviceName().c_str());

    // Load and send initial backflush settings (same for both groups initially)
    auto groupOneBackflush = (uint16_t)preferenceHelper->getULong(
        PreferenceKey::BackflushActivationTimeMs, 500);
    auto groupTwoBackflush = (uint16_t)preferenceHelper->getULong(
        PreferenceKey::BackflushDeactivationTimeMs, 500);

    bleService->updateBackflushSettings(groupOneBackflush, groupTwoBackflush);
    bleService->updateVolumetricSettings();

    // Load and send initial auto-backflush settings
    auto extractDuration = (uint16_t)preferenceHelper->getULong(
        PreferenceKey::AutoBackflushExtractDurationMs, DEFAULT_EXTRACT_DURATION_MS);
    auto pauseDuration = (uint16_t)preferenceHelper->getULong(
        PreferenceKey::AutoBackflushPauseDurationMs, DEFAULT_PAUSE_DURATION_MS);
    auto cycles = (uint16_t)preferenceHelper->getULong(
        PreferenceKey::AutoBackflushCycles, DEFAULT_TOTAL_CYCLES);

    bleService->updateAutoBackflushSettings(extractDuration, pauseDuration, cycles);

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

    // Update boiler state (read boiler fill level and raw ADC value)
    const auto &boilerHandler = stateHandler->getBoilerHandler();
    uint8_t boilerState = state->isFillingBoiler ? 1 : 0;
    bleService->updateBoilerState(state->isFillingBoiler, boilerState, boilerHandler.getBoilerProbeRawValue());

    // Update progress if extraction is active
    if (state->groupOneIsExtracting)
    {
        const auto &groupOne = stateHandler->getGroupOneHandler();
        bleService->updateGroupProgress(true, groupOne.getCurrentPulses(), groupOne.getTargetPulses());
    }

    if (state->groupTwoIsExtracting)
    {
        const auto &groupTwo = stateHandler->getGroupTwoHandler();
        bleService->updateGroupProgress(false, groupTwo.getCurrentPulses(), groupTwo.getTargetPulses());
    }

    // Periodically refresh settings
    unsigned long currentTime = millis();
    if (currentTime - lastSettingsUpdateTime > SETTINGS_UPDATE_INTERVAL)
    {
        auto groupOneBackflush = (uint16_t)preferenceHelper->getULong(
            PreferenceKey::BackflushActivationTimeMs, 500);
        auto groupTwoBackflush = (uint16_t)preferenceHelper->getULong(
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
