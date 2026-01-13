//
// Enhanced BLE Service with comprehensive machine control
//

#include "BLEService.h"

void BrewPilotBLEService::begin(const char *deviceName)
{
    // Initialize BLE
    NimBLEDevice::init(deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create server
    pServer = NimBLEDevice::createServer();

    // Create service
    pService = pServer->createService(BREWPILOT_SERVICE_UUID);

    // Create state characteristic (read + notify)
    pStateCharacteristic = pService->createCharacteristic(
        STATE_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    pStateCharacteristic->setCallbacks(nullptr);

    // Create boiler state characteristic (read + notify)
    pBoilerStateCharacteristic = pService->createCharacteristic(
        BOILER_STATE_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    pBoilerStateCharacteristic->setCallbacks(nullptr);

    // Create group one progress characteristic (read + notify)
    pGroupOneProgressCharacteristic = pService->createCharacteristic(
        GROUP_ONE_PROGRESS_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    pGroupOneProgressCharacteristic->setCallbacks(nullptr);

    // Create group two progress characteristic (read + notify)
    pGroupTwoProgressCharacteristic = pService->createCharacteristic(
        GROUP_TWO_PROGRESS_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    pGroupTwoProgressCharacteristic->setCallbacks(nullptr);

    // Create group one backflush mode characteristic (read + write)
    pGroupOneBackflushCharacteristic = pService->createCharacteristic(
        GROUP_ONE_BACKFLUSH_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    UInt16SettingCallback *groupOneBackflushCallback = new UInt16SettingCallback(
        preferenceHelper, PreferenceKey::BackflushActivationTimeMs);
    pGroupOneBackflushCharacteristic->setCallbacks(groupOneBackflushCallback);
    callbackPtrs.push_back(groupOneBackflushCallback);

    // Create group two backflush mode characteristic (read + write)
    pGroupTwoBackflushCharacteristic = pService->createCharacteristic(
        GROUP_TWO_BACKFLUSH_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    UInt16SettingCallback *groupTwoBackflushCallback = new UInt16SettingCallback(
        preferenceHelper, PreferenceKey::BackflushDeactivationTimeMs);
    pGroupTwoBackflushCharacteristic->setCallbacks(groupTwoBackflushCallback);
    callbackPtrs.push_back(groupTwoBackflushCallback);

    // Create volumetric settings characteristics (READ ONLY)
    pLeftSingleCharacteristic = pService->createCharacteristic(
        LEFT_SINGLE_ESPRESSO_UUID,
        NIMBLE_PROPERTY::READ);
    pLeftSingleCharacteristic->setCallbacks(nullptr);

    pLeftDoubleCharacteristic = pService->createCharacteristic(
        LEFT_DOUBLE_ESPRESSO_UUID,
        NIMBLE_PROPERTY::READ);
    pLeftDoubleCharacteristic->setCallbacks(nullptr);

    pRightSingleCharacteristic = pService->createCharacteristic(
        RIGHT_SINGLE_ESPRESSO_UUID,
        NIMBLE_PROPERTY::READ);
    pRightSingleCharacteristic->setCallbacks(nullptr);

    pRightDoubleCharacteristic = pService->createCharacteristic(
        RIGHT_DOUBLE_ESPRESSO_UUID,
        NIMBLE_PROPERTY::READ);
    pRightDoubleCharacteristic->setCallbacks(nullptr);

    pTeaWaterCharacteristic = pService->createCharacteristic(
        TEA_WATER_DURATION_UUID,
        NIMBLE_PROPERTY::READ);
    pTeaWaterCharacteristic->setCallbacks(nullptr);

    // Create programming mode characteristic (read + write)
    pProgrammingModeCharacteristic = pService->createCharacteristic(
        PROGRAMMING_MODE_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    ProgrammingModeCallback *progModeCallback = new ProgrammingModeCallback(&state->isInProgrammingMode);
    pProgrammingModeCharacteristic->setCallbacks(progModeCallback);
    callbackPtrs.push_back(progModeCallback);

    // Start service
    pService->start();

    // Create advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BREWPILOT_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();

    Serial.println("BLE Service started - BrewPilot is advertising with comprehensive characteristics");
}

void BrewPilotBLEService::updateState()
{
    if (pStateCharacteristic != nullptr)
    {
        BLEStateData stateData{
            state->isFillingBoiler,
            state->groupOneIsExtracting,
            state->groupTwoIsExtracting,
            state->isExtractingTeaWater,
            state->isInProgrammingMode};

        pStateCharacteristic->setValue((uint8_t *)&stateData, sizeof(BLEStateData));
        pStateCharacteristic->notify();
    }
}

void BrewPilotBLEService::updateBoilerState(bool isFilling, uint16_t probeValue, uint8_t boilerState)
{
    if (pBoilerStateCharacteristic != nullptr)
    {
        BLEBoilerStateData data{isFilling, probeValue, boilerState};
        pBoilerStateCharacteristic->setValue((uint8_t *)&data, sizeof(BLEBoilerStateData));
        pBoilerStateCharacteristic->notify();
    }
}

void BrewPilotBLEService::updateGroupProgress(bool isGroupOne, uint32_t currentPulses, uint32_t targetPulses)
{
    NimBLECharacteristic *progressChar = isGroupOne ? pGroupOneProgressCharacteristic : pGroupTwoProgressCharacteristic;
    if (progressChar != nullptr)
    {
        BLEProgressData data{currentPulses, targetPulses};
        progressChar->setValue((uint8_t *)&data, sizeof(BLEProgressData));
        progressChar->notify();
    }
}

void BrewPilotBLEService::updateVolumetricSettings()
{
    if (pLeftSingleCharacteristic != nullptr)
    {
        uint32_t val = volumetricsHelper->getFlowMeterSetting(GroupHeadButtonEvent::LEFT_SINGLE_ESPRESSO);
        uint8_t data[4] = {(uint8_t)(val & 0xFF), (uint8_t)((val >> 8) & 0xFF),
                           (uint8_t)((val >> 16) & 0xFF), (uint8_t)((val >> 24) & 0xFF)};
        pLeftSingleCharacteristic->setValue(data, sizeof(data));
    }
    if (pLeftDoubleCharacteristic != nullptr)
    {
        uint32_t val = volumetricsHelper->getFlowMeterSetting(GroupHeadButtonEvent::LEFT_DOUBLE_ESPRESSO);
        uint8_t data[4] = {(uint8_t)(val & 0xFF), (uint8_t)((val >> 8) & 0xFF),
                           (uint8_t)((val >> 16) & 0xFF), (uint8_t)((val >> 24) & 0xFF)};
        pLeftDoubleCharacteristic->setValue(data, sizeof(data));
    }
    if (pRightSingleCharacteristic != nullptr)
    {
        uint32_t val = volumetricsHelper->getFlowMeterSetting(GroupHeadButtonEvent::RIGHT_SINGLE_ESPRESSO);
        uint8_t data[4] = {(uint8_t)(val & 0xFF), (uint8_t)((val >> 8) & 0xFF),
                           (uint8_t)((val >> 16) & 0xFF), (uint8_t)((val >> 24) & 0xFF)};
        pRightSingleCharacteristic->setValue(data, sizeof(data));
    }
    if (pRightDoubleCharacteristic != nullptr)
    {
        uint32_t val = volumetricsHelper->getFlowMeterSetting(GroupHeadButtonEvent::RIGHT_DOUBLE_ESPRESSO);
        uint8_t data[4] = {(uint8_t)(val & 0xFF), (uint8_t)((val >> 8) & 0xFF),
                           (uint8_t)((val >> 16) & 0xFF), (uint8_t)((val >> 24) & 0xFF)};
        pRightDoubleCharacteristic->setValue(data, sizeof(data));
    }
    if (pTeaWaterCharacteristic != nullptr)
    {
        uint32_t val = volumetricsHelper->getTeaWaterSetting();
        uint8_t data[4] = {(uint8_t)(val & 0xFF), (uint8_t)((val >> 8) & 0xFF),
                           (uint8_t)((val >> 16) & 0xFF), (uint8_t)((val >> 24) & 0xFF)};
        pTeaWaterCharacteristic->setValue(data, sizeof(data));
    }
}

void BrewPilotBLEService::updateProgrammingMode(bool enabled)
{
    if (pProgrammingModeCharacteristic != nullptr)
    {
        uint8_t data[1] = {enabled ? 1 : 0};
        pProgrammingModeCharacteristic->setValue(data, sizeof(data));
    }
}

void BrewPilotBLEService::updateBackflushSettings(uint16_t groupOneBackflush, uint16_t groupTwoBackflush)
{
    if (pGroupOneBackflushCharacteristic != nullptr)
    {
        uint8_t data[2] = {(uint8_t)(groupOneBackflush & 0xFF), (uint8_t)((groupOneBackflush >> 8) & 0xFF)};
        pGroupOneBackflushCharacteristic->setValue(data, sizeof(data));
    }
    if (pGroupTwoBackflushCharacteristic != nullptr)
    {
        uint8_t data[2] = {(uint8_t)(groupTwoBackflush & 0xFF), (uint8_t)((groupTwoBackflush >> 8) & 0xFF)};
        pGroupTwoBackflushCharacteristic->setValue(data, sizeof(data));
    }
}
