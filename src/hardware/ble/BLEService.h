//
// Created for BLE communication
//

#ifndef BREWPILOT_BLE_SERVICE_H
#define BREWPILOT_BLE_SERVICE_H

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLECharacteristic.h>
#include "hardware/state/State.h"
#include "utils/PreferenceHelper.h"
#include "utils/VolumetricsHelper.h"

// UUIDs for BrewPilot service
#define BREWPILOT_SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define STATE_CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1"
#define BACKFLUSH_ACTIVATION_UUID "12345678-1234-5678-1234-56789abcdef2"
#define BACKFLUSH_DEACTIVATION_UUID "12345678-1234-5678-1234-56789abcdef3"

// State structure to send over BLE (packed binary format)
struct BLEStateData
{
    bool isFillingBoiler;
    bool groupOneIsExtracting;
    bool groupTwoIsExtracting;
    bool isExtractingTeaWater;
    bool isInProgrammingMode;
} __attribute__((packed));

class BLEStateCallback : public NimBLECharacteristicCallbacks
{
private:
    PreferenceHelper *preferenceHelper;

public:
    BLEStateCallback(PreferenceHelper *preferenceHelper)
        : preferenceHelper(preferenceHelper) {}

    void onWrite(NimBLECharacteristic *pCharacteristic) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() >= sizeof(uint16_t))
        {
            uint16_t value = (uint16_t)rxValue[0] | ((uint16_t)rxValue[1] << 8);
            preferenceHelper->setULong(PreferenceKey::BackflushActivationTimeMs, value);
            Serial.printf("BLE: Set backflush activation time to %u ms\n", value);
        }
    }
};

class BLEDeactivationCallback : public NimBLECharacteristicCallbacks
{
private:
    PreferenceHelper *preferenceHelper;

public:
    BLEDeactivationCallback(PreferenceHelper *preferenceHelper)
        : preferenceHelper(preferenceHelper) {}

    void onWrite(NimBLECharacteristic *pCharacteristic) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() >= sizeof(uint16_t))
        {
            uint16_t value = (uint16_t)rxValue[0] | ((uint16_t)rxValue[1] << 8);
            preferenceHelper->setULong(PreferenceKey::BackflushDeactivationTimeMs, value);
            Serial.printf("BLE: Set backflush deactivation time to %u ms\n", value);
        }
    }
};

class BLEService
{
private:
    NimBLEServer *pServer;
    NimBLEService *pService;
    NimBLECharacteristic *pStateCharacteristic;
    NimBLECharacteristic *pBackflushActivationCharacteristic;
    NimBLECharacteristic *pBackflushDeactivationCharacteristic;
    
    State *state;
    PreferenceHelper *preferenceHelper;
    
    BLEStateCallback *activationCallback;
    BLEDeactivationCallback *deactivationCallback;

public:
    BLEService(State *state, PreferenceHelper *preferenceHelper)
        : state(state), preferenceHelper(preferenceHelper),
          activationCallback(nullptr), deactivationCallback(nullptr) {}

    void begin(const char *deviceName = "BrewPilot")
    {
        // Initialize BLE
        NimBLEDevice::init(deviceName);
        
        // Create server
        pServer = NimBLEDevice::createServer();
        
        // Create service
        pService = pServer->createService(BREWPILOT_SERVICE_UUID);
        
        // Create state characteristic (read + notify)
        pStateCharacteristic = pService->createCharacteristic(
            STATE_CHARACTERISTIC_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
        );
        pStateCharacteristic->setCallbacks(nullptr);
        
        // Create backflush activation characteristic (read + write)
        pBackflushActivationCharacteristic = pService->createCharacteristic(
            BACKFLUSH_ACTIVATION_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        activationCallback = new BLEStateCallback(preferenceHelper);
        pBackflushActivationCharacteristic->setCallbacks(activationCallback);
        
        // Create backflush deactivation characteristic (read + write)
        pBackflushDeactivationCharacteristic = pService->createCharacteristic(
            BACKFLUSH_DEACTIVATION_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        deactivationCallback = new BLEDeactivationCallback(preferenceHelper);
        pBackflushDeactivationCharacteristic->setCallbacks(deactivationCallback);
        
        // Start service
        pService->start();
        
        // Create advertising
        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(BREWPILOT_SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone compatibility
        pAdvertising->setMinPreferred(0x12);
        
        // Start advertising
        NimBLEDevice::startAdvertising();
        Serial.println("BLE Service started - BrewPilot is advertising");
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
                state->isInProgrammingMode
            };
            
            pStateCharacteristic->setValue((uint8_t *)&stateData, sizeof(BLEStateData));
            pStateCharacteristic->notify();
        }
    }

    void updateBackflushSettings(uint16_t activationTimeMs, uint16_t deactivationTimeMs)
    {
        if (pBackflushActivationCharacteristic != nullptr)
        {
            uint8_t data[2];
            data[0] = (uint8_t)(activationTimeMs & 0xFF);
            data[1] = (uint8_t)((activationTimeMs >> 8) & 0xFF);
            pBackflushActivationCharacteristic->setValue(data, sizeof(data));
        }
        
        if (pBackflushDeactivationCharacteristic != nullptr)
        {
            uint8_t data[2];
            data[0] = (uint8_t)(deactivationTimeMs & 0xFF);
            data[1] = (uint8_t)((deactivationTimeMs >> 8) & 0xFF);
            pBackflushDeactivationCharacteristic->setValue(data, sizeof(data));
        }
    }
};

#endif // BREWPILOT_BLE_SERVICE_H
