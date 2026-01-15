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
#include "utils/DeviceNameHelper.h"
#include "structs/VolumetricSettings.h"

// Main UUIDs
constexpr const char *BREWPILOT_SERVICE_UUID = "d330fed9-bbcc-4afe-89bc-367caef99ccc";

// Boiler state enum
enum class BoilerHeatingState : uint8_t
{
    READY = 0,       // Boiler is above target and filled
    BELOW_TARGET = 1 // Boiler is below target and filling
};

// State characteristics
constexpr const char *STATE_CHARACTERISTIC_UUID = "9ea67743-2f51-4152-9ffc-f373d87a8671";
constexpr const char *BOILER_STATE_UUID = "840ce997-807e-4552-976d-0f3240456e42";
constexpr const char *GROUP_ONE_PROGRESS_UUID = "c1301773-8bec-49e7-945d-ee4f32f50d3e";
constexpr const char *GROUP_TWO_PROGRESS_UUID = "7be528a0-d8b8-49ab-a742-04e4902f52cd";

// Settings characteristics - Backflush (per group)
constexpr const char *GROUP_ONE_BACKFLUSH_UUID = "c2343aa3-3157-4e6a-b2db-063c33f3a6fc";
constexpr const char *GROUP_TWO_BACKFLUSH_UUID = "0b33efcc-6bf2-48a4-b6f7-a325639e66f0";

// Settings characteristics - Volumetrics (dose settings - READ ONLY)
constexpr const char *LEFT_SINGLE_ESPRESSO_UUID = "85cd1e73-a8d9-49d0-bc06-e0219fe63673";
constexpr const char *LEFT_DOUBLE_ESPRESSO_UUID = "4d5dddf2-b629-4ab3-bbca-a5cd12b791d0";
constexpr const char *RIGHT_SINGLE_ESPRESSO_UUID = "56cb25ad-061f-434a-9910-f28fe0e2ea0d";
constexpr const char *RIGHT_DOUBLE_ESPRESSO_UUID = "96e0682c-51ce-4422-a176-397d55b24e3a";
constexpr const char *TEA_WATER_DURATION_UUID = "657612be-c669-46ca-b7e6-22e62b6bb08d";

// Settings characteristics - Auto Backflush timing
constexpr const char *AUTO_BACKFLUSH_EXTRACT_DUR_UUID = "ed66aa43-04a2-470c-a121-37275ac69ef2";
constexpr const char *AUTO_BACKFLUSH_PAUSE_DUR_UUID = "4d47f2bf-2a36-4e49-8c0c-a55ee9d6345b";
constexpr const char *AUTO_BACKFLUSH_CYCLES_UUID = "fcd919f1-f0b6-4da6-87c9-ba41e547c561";

// Diagnostic characteristics (read-only)
constexpr const char *DEVICE_NAME_UUID = "5ac34884-ee93-4661-b92d-58c06f81b262";
constexpr const char *FIRMWARE_VERSION_UUID = "59465bf3-d0bb-4180-a797-cf9575321736";
constexpr const char *MACHINE_TYPE_UUID = "a6d2b889-1c62-42fb-bda9-592772076095";

// Control characteristics
constexpr const char *PROGRAMMING_MODE_UUID = "f35199a3-0021-496a-ab48-1d197eeae2b9";

// Device customization
constexpr const char *CUSTOM_DEVICE_NAME_UUID = "c18a1626-e3cd-45bf-be37-f667caac78b2";

// Server callbacks for connection events
class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer) override
    {
        Serial.println("BLE: Client connected");
    }

    void onDisconnect(NimBLEServer *pServer) override
    {
        Serial.println("BLE: Client disconnected");
        // Restart advertising
        NimBLEDevice::startAdvertising();
        Serial.println("BLE: Advertising restarted");
    }
};

// State structures for packing
struct BLEStateData
{
    uint8_t isFillingBoiler;
    uint8_t groupOneIsExtracting;
    uint8_t groupTwoIsExtracting;
    uint8_t isExtractingTeaWater;
    uint8_t isInProgrammingMode;
} __attribute__((packed));

struct BLEBoilerStateData
{
    uint8_t isFillingBoiler;
    uint16_t boilerProbeRawValue;
    uint8_t boilerState; // 0=READY, 1=BELOW_TARGET
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

// Callback for custom device name
class CustomDeviceNameCallback : public NimBLECharacteristicCallbacks
{
private:
    DeviceNameHelper *deviceNameHelper;

public:
    explicit CustomDeviceNameCallback(DeviceNameHelper *deviceNameHelper)
        : deviceNameHelper(deviceNameHelper) {}

    void onWrite(NimBLECharacteristic *pCharacteristic) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0 && rxValue.length() <= 32)
        {
            deviceNameHelper->setDeviceNameSuffix(rxValue.c_str());
            Serial.printf("BLE: Custom device name suffix updated to: %s\n", rxValue.c_str());
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

    // Device customization (read + write)
    NimBLECharacteristic *pCustomDeviceNameCharacteristic;

    // Device info characteristics (read-only)
    NimBLECharacteristic *pDeviceNameCharacteristic;
    NimBLECharacteristic *pFirmwareVersionCharacteristic;
    NimBLECharacteristic *pMachineTypeCharacteristic;

    State *state;
    PreferenceHelper *preferenceHelper;
    VolumetricsHelper *volumetricsHelper;
    DeviceNameHelper *deviceNameHelper;

    std::vector<NimBLECharacteristicCallbacks *> callbackPtrs;

public:
    BrewPilotBLEService(State *state, PreferenceHelper *preferenceHelper, VolumetricsHelper *volumetricsHelper, DeviceNameHelper *deviceNameHelper)
        : state(state), preferenceHelper(preferenceHelper), volumetricsHelper(volumetricsHelper), deviceNameHelper(deviceNameHelper),
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

    void begin(const char *deviceName = "BrewPilot");

    void updateState();

    void updateBoilerState(bool isFilling, uint16_t probeValue, uint8_t boilerState);

    void updateGroupProgress(bool isGroupOne, uint32_t currentPulses, uint32_t targetPulses);

    void updateVolumetricSettings();

    void updateProgrammingMode(bool enabled);

    void updateBackflushSettings(uint16_t groupOneBackflush, uint16_t groupTwoBackflush);

    void updateCustomDeviceName(const char *suffix);
};

#endif // BREWPILOT_BLE_SERVICE_H