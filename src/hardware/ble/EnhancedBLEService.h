//
// Enhanced BLE Service with comprehensive machine control
//

#ifndef BREWPILOT_ENHANCED_BLE_SERVICE_H
#define BREWPILOT_ENHANCED_BLE_SERVICE_H

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
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

// Settings characteristics - Backflush
#define BACKFLUSH_ACTIVATION_UUID "12345678-1234-5678-1234-56789abcdef5"
#define BACKFLUSH_DEACTIVATION_UUID "12345678-1234-5678-1234-56789abcdef6"

// Settings characteristics - Volumetrics (dose settings)
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
            Serial.printf("BLE: Updated setting via BLE, stored value: %lu\n", value);
        }
    }
};

// Callback for uint16 settings (backflush times)
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
            Serial.printf("BLE: Set uint16 setting to %u ms\n", value);
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

#endif // BREWPILOT_ENHANCED_BLE_SERVICE_H
