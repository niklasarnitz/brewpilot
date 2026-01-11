//
// Enhanced BLE Service with comprehensive machine control
//

#ifndef BREWPILOT_BLE_SERVICE_H
#define BREWPILOT_BLE_SERVICE_H

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEService.h>
#include <NimBLECharacteristic.h>
#include "hardware/state/State.h"
#include "utils/PreferenceHelper.h"
#include "utils/VolumetricsHelper.h"
#include "structs/VolumetricSettings.h"

// Main UUIDs
#define BREWPILOT_SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"

// State characteristics
#define STATE_CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1"
#define BOILER_STATE_UUID "12345678-1234-5678-1234-56789abcdef2"
#define GROUP_ONE_PROGRESS_UUID "12345678-1234-5678-1234-56789abcdef3"
#define GROUP_TWO_PROGRESS_UUID "12345678-1234-5678-1234-56789abcdef4"

// Settings characteristics - Backflush (per group)
#define GROUP_ONE_BACKFLUSH_UUID "12345678-1234-5678-1234-56789abcdef5"
#define GROUP_TWO_BACKFLUSH_UUID "12345678-1234-5678-1234-56789abcdef6"

// Settings characteristics - Volumetrics (dose settings - READ ONLY)
#define LEFT_SINGLE_ESPRESSO_UUID "12345678-1234-5678-1234-56789abcdef7"
#define LEFT_DOUBLE_ESPRESSO_UUID "12345678-1234-5678-1234-56789abcdef8"
#define RIGHT_SINGLE_ESPRESSO_UUID "12345678-1234-5678-1234-56789abcdef9"
#define RIGHT_DOUBLE_ESPRESSO_UUID "12345678-1234-5678-1234-56789abcdef10"
#define TEA_WATER_DURATION_UUID "12345678-1234-5678-1234-56789abcdef11"

// Settings characteristics - Auto Backflush timing
#define AUTO_BACKFLUSH_EXTRACT_DUR_UUID "12345678-1234-5678-1234-56789abcdef12"
#define AUTO_BACKFLUSH_PAUSE_DUR_UUID "12345678-1234-5678-1234-56789abcdef13"
#define AUTO_BACKFLUSH_CYCLES_UUID "12345678-1234-5678-1234-56789abcdef14"

// Diagnostic characteristics (read-only)
#define DEVICE_NAME_UUID "12345678-1234-5678-1234-56789abcdef15"
#define FIRMWARE_VERSION_UUID "12345678-1234-5678-1234-56789abcdef16"
#define MACHINE_TYPE_UUID "12345678-1234-5678-1234-56789abcdef17"

// Control characteristics
#define PROGRAMMING_MODE_UUID "12345678-1234-5678-1234-56789abcdef18"

// State structures for packing
struct BLEStateData
{
    bool isFillingBoiler;
    bool groupOneIsExtracting;
    bool groupTwoIsExtracting;
    bool isExtractingTeaWater;
    bool isInProgrammingMode;
} __attribute__((packed));

struct BLEBoilerStateData
{
    bool isFillingBoiler;
    uint16_t boilerProbeRawValue;
    uint8_t boilerState; // 0=above_filled, 1=above_filling, 2=below_target
} __attribute__((packed));

struct BLEProgressData
{
    uint32_t currentPulses;
    uint32_t targetPulses;
} __attribute__((packed));

// Callback for volumetric settings writes
class VolumetricWriteCallback : public NimBLECharacteristicCallbacks
{
private:
    PreferenceHelper *preferenceHelper;
    PreferenceKey preferenceKey;

public:
    VolumetricWriteCallback(PreferenceHelper *preferenceHelper, PreferenceKey key)
        : preferenceHelper(preferenceHelper), preferenceKey(key) {}

    void onWrite(NimBLECharacteristic *pCharacteristic) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() >= sizeof(uint32_t))
        {
            uint32_t value = (uint32_t)rxValue[0] |
                             ((uint32_t)rxValue[1] << 8) |
                             ((uint32_t)rxValue[2] << 16) |
                             ((uint32_t)rxValue[3] << 24);
            preferenceHelper->setULong(preferenceKey, value);
            Serial.printf("BLE: Updated volumetric setting, stored value: %lu\n", value);
        }
    }
};

// Callback for uint16 settings (backflush times, auto-backflush durations)
class UInt16SettingCallback : public NimBLECharacteristicCallbacks
{
private:
    PreferenceHelper *preferenceHelper;
    PreferenceKey preferenceKey;

public:
    UInt16SettingCallback(PreferenceHelper *preferenceHelper, PreferenceKey key)
        : preferenceHelper(preferenceHelper), preferenceKey(key) {}

    void onWrite(NimBLECharacteristic *pCharacteristic) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() >= sizeof(uint16_t))
        {
            uint16_t value = (uint16_t)rxValue[0] | ((uint16_t)rxValue[1] << 8);
            preferenceHelper->setULong(preferenceKey, value);
            Serial.printf("BLE: Set uint16 setting to %u\n", value);
        }
    }
};

// Callback for programming mode control
class ProgrammingModeCallback : public NimBLECharacteristicCallbacks
{
private:
    bool *isInProgrammingMode;

public:
    ProgrammingModeCallback(bool *isInProgrammingMode)
        : isInProgrammingMode(isInProgrammingMode) {}

    void onWrite(NimBLECharacteristic *pCharacteristic) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0)
        {
            bool enable = rxValue[0] != 0;
            *isInProgrammingMode = enable;
            Serial.printf("BLE: Programming mode %s\n", enable ? "ENABLED" : "DISABLED");
        }
    }
};

class BrewPilotBLEService
{
private:
    NimBLEServer *pServer;
    NimBLEService *pService;

    // State characteristics (read + notify)
    NimBLECharacteristic *pStateCharacteristic;
    NimBLECharacteristic *pBoilerStateCharacteristic;
    NimBLECharacteristic *pGroupOneProgressCharacteristic;
    NimBLECharacteristic *pGroupTwoProgressCharacteristic;

    // Backflush mode characteristics (read + write per group)
    NimBLECharacteristic *pGroupOneBackflushCharacteristic;
    NimBLECharacteristic *pGroupTwoBackflushCharacteristic;

    // Volumetric settings (read-only)
    NimBLECharacteristic *pLeftSingleCharacteristic;
    NimBLECharacteristic *pLeftDoubleCharacteristic;
    NimBLECharacteristic *pRightSingleCharacteristic;
    NimBLECharacteristic *pRightDoubleCharacteristic;
    NimBLECharacteristic *pTeaWaterCharacteristic;

    // Control characteristics (read + write)
    NimBLECharacteristic *pProgrammingModeCharacteristic;

    State *state;
    PreferenceHelper *preferenceHelper;
    VolumetricsHelper *volumetricsHelper;

    std::vector<NimBLECharacteristicCallbacks *> callbackPtrs;

public:
    BrewPilotBLEService(State *state, PreferenceHelper *preferenceHelper, VolumetricsHelper *volumetricsHelper)
        : state(state), preferenceHelper(preferenceHelper), volumetricsHelper(volumetricsHelper),
          pServer(nullptr), pService(nullptr) {}

    ~BrewPilotBLEService()
    {
        // Delete all dynamically allocated callbacks
        for (NimBLECharacteristicCallbacks *callback : callbackPtrs)
        {
            delete callback;
        }
        callbackPtrs.clear();
    }

    void begin(const char *deviceName = "BrewPilot")
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

    void updateState()
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

    void updateBoilerState(bool isFilling, uint16_t probeValue, uint8_t boilerState)
    {
        if (pBoilerStateCharacteristic != nullptr)
        {
            BLEBoilerStateData data{isFilling, probeValue, boilerState};
            pBoilerStateCharacteristic->setValue((uint8_t *)&data, sizeof(BLEBoilerStateData));
            pBoilerStateCharacteristic->notify();
        }
    }

    void updateGroupProgress(bool isGroupOne, uint32_t currentPulses, uint32_t targetPulses)
    {
        NimBLECharacteristic *progressChar = isGroupOne ? pGroupOneProgressCharacteristic : pGroupTwoProgressCharacteristic;
        if (progressChar != nullptr)
        {
            BLEProgressData data{currentPulses, targetPulses};
            progressChar->setValue((uint8_t *)&data, sizeof(BLEProgressData));
            progressChar->notify();
        }
    }

    void updateVolumetricSettings()
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

    void updateProgrammingMode(bool enabled)
    {
        if (pProgrammingModeCharacteristic != nullptr)
        {
            uint8_t data[1] = {enabled ? 1 : 0};
            pProgrammingModeCharacteristic->setValue(data, sizeof(data));
        }
    }

    void updateBackflushSettings(uint16_t groupOneBackflush, uint16_t groupTwoBackflush)
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
};

#endif // BREWPILOT_BLE_SERVICE_H
