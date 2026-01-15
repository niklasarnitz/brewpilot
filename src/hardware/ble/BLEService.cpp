//
// Enhanced BLE Service with comprehensive machine control
//

#include "BLEService.h"
#include "UserConfig.h"

void BrewPilotBLEService::begin(const char *deviceName)
{
    // Initialize BLE
    NimBLEDevice::init(deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create service
    pService = pServer->createService(BREWPILOT_SERVICE_UUID);

    Serial.printf("BLE: Starting service with device name: %s\n", deviceName);

    // Create state characteristic (read + notify)
    pStateCharacteristic = pService->createCharacteristic(
        STATE_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    // Initialize with all false state
    BLEStateData initialState{0, 0, 0, 0, 0};
    pStateCharacteristic->setValue((uint8_t *)&initialState, sizeof(BLEStateData));
    pStateCharacteristic->setCallbacks(nullptr);

    // Create boiler state characteristic (read + notify)
    pBoilerStateCharacteristic = pService->createCharacteristic(
        BOILER_STATE_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    // Initialize boiler state
    BLEBoilerStateData initialBoilerState{0, 0, 0};
    pBoilerStateCharacteristic->setValue((uint8_t *)&initialBoilerState, sizeof(BLEBoilerStateData));
    pBoilerStateCharacteristic->setCallbacks(nullptr);

    // Create group one progress characteristic (read + notify)
    pGroupOneProgressCharacteristic = pService->createCharacteristic(
        GROUP_ONE_PROGRESS_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    // Initialize progress
    BLEProgressData initialProgress{0, 0};
    pGroupOneProgressCharacteristic->setValue((uint8_t *)&initialProgress, sizeof(BLEProgressData));
    pGroupOneProgressCharacteristic->setCallbacks(nullptr);

    // Create group two progress characteristic (read + notify)
    pGroupTwoProgressCharacteristic = pService->createCharacteristic(
        GROUP_TWO_PROGRESS_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    // Initialize progress
    pGroupTwoProgressCharacteristic->setValue((uint8_t *)&initialProgress, sizeof(BLEProgressData));
    pGroupTwoProgressCharacteristic->setCallbacks(nullptr);

    // Create group one backflush mode characteristic (read + write)
    pGroupOneBackflushCharacteristic = pService->createCharacteristic(
        GROUP_ONE_BACKFLUSH_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    auto groupOneBackflushCallback = std::make_unique<UInt16SettingCallback>(
        preferenceHelper, PreferenceKey::BackflushActivationTimeMs);
    pGroupOneBackflushCharacteristic->setCallbacks(groupOneBackflushCallback.get());
    callbackPtrs.push_back(std::move(groupOneBackflushCallback));

    // Create group two backflush mode characteristic (read + write)
    pGroupTwoBackflushCharacteristic = pService->createCharacteristic(
        GROUP_TWO_BACKFLUSH_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    auto groupTwoBackflushCallback = std::make_unique<UInt16SettingCallback>(
        preferenceHelper, PreferenceKey::BackflushDeactivationTimeMs);
    pGroupTwoBackflushCharacteristic->setCallbacks(groupTwoBackflushCallback.get());
    callbackPtrs.push_back(std::move(groupTwoBackflushCallback));

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
    auto progModeCallback = std::make_unique<ProgrammingModeCallback>(&state->isInProgrammingMode);
    pProgrammingModeCharacteristic->setCallbacks(progModeCallback.get());
    callbackPtrs.push_back(std::move(progModeCallback));

    // Create custom device name characteristic (read + write)
    pCustomDeviceNameCharacteristic = pService->createCharacteristic(
        CUSTOM_DEVICE_NAME_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    auto customNameCallback = std::make_unique<CustomDeviceNameCallback>(deviceNameHelper);
    pCustomDeviceNameCharacteristic->setCallbacks(customNameCallback.get());
    callbackPtrs.push_back(std::move(customNameCallback));
    // Set initial value to current suffix with explicit length
    String suffix = deviceNameHelper->getDeviceNameSuffix();
    pCustomDeviceNameCharacteristic->setValue(reinterpret_cast<const uint8_t *>(suffix.c_str()), suffix.length());

    // Create device info characteristics (read-only)
    pDeviceNameCharacteristic = pService->createCharacteristic(
        DEVICE_NAME_UUID,
        NIMBLE_PROPERTY::READ);
    pDeviceNameCharacteristic->setValue(reinterpret_cast<const uint8_t *>(deviceName), strlen(deviceName));
    pDeviceNameCharacteristic->setCallbacks(nullptr);

    pFirmwareVersionCharacteristic = pService->createCharacteristic(
        FIRMWARE_VERSION_UUID,
        NIMBLE_PROPERTY::READ);
    const char *fwVersion = "1.0.0";
    pFirmwareVersionCharacteristic->setValue(reinterpret_cast<const uint8_t *>(fwVersion), strlen(fwVersion));
    pFirmwareVersionCharacteristic->setCallbacks(nullptr);

    pMachineTypeCharacteristic = pService->createCharacteristic(
        MACHINE_TYPE_UUID,
        NIMBLE_PROPERTY::READ);
    // Set machine type from user configuration
    const char *machineTypeStr = (HARDWARE_MODEL == MachineType::LA_CIMBALI_M29_SELECT)
                                     ? "LA_CIMBALI_M29_SELECT"
                                     : "RANCILIO_S27";
    pMachineTypeCharacteristic->setValue(reinterpret_cast<const uint8_t *>(machineTypeStr), strlen(machineTypeStr));
    pMachineTypeCharacteristic->setCallbacks(nullptr);

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
            (uint8_t)(state->isFillingBoiler ? 1 : 0),
            (uint8_t)(state->groupOneIsExtracting ? 1 : 0),
            (uint8_t)(state->groupTwoIsExtracting ? 1 : 0),
            (uint8_t)(state->isExtractingTeaWater ? 1 : 0),
            (uint8_t)(state->isInProgrammingMode ? 1 : 0)};

        pStateCharacteristic->setValue((uint8_t *)&stateData, sizeof(BLEStateData));
        pStateCharacteristic->notify();
    }
}

void BrewPilotBLEService::updateBoilerState(bool isFilling, uint16_t probeValue, uint8_t boilerState)
{
    if (pBoilerStateCharacteristic != nullptr)
    {
        BLEBoilerStateData data{(uint8_t)(isFilling ? 1 : 0), probeValue, boilerState};
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
        uint8_t data = enabled ? 1 : 0;
        pProgrammingModeCharacteristic->setValue(&data, 1);
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

void BrewPilotBLEService::updateCustomDeviceName(const char *suffix)
{
    if (pCustomDeviceNameCharacteristic != nullptr && suffix != nullptr)
    {
        pCustomDeviceNameCharacteristic->setValue(suffix);
    }
}
