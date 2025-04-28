#ifndef BREWPILOT_BLEMANAGER_H
#define BREWPILOT_BLEMANAGER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <queue>
#include <string>
#include <mutex>
#include <Arduino.h>

#include "../../hardware/state/State.h"
#include "../../utils/VolumetricsHelper.h"

// BLE UUIDs
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define STATE_CHAR_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define VOLUMETRIC_CHAR_UUID "8e22f82c-5dd5-4f7b-9b44-a1d5bcc83357"
#define EVENT_LOG_CHAR_UUID "2c53f395-c07f-4062-9b7c-adedd5763b2c"

class BLEManager
{
private:
    State *_state;
    VolumetricsHelper *_volumetricsHelper;

    BLEServer *pServer = nullptr;
    BLECharacteristic *pStateCharacteristic = nullptr;
    BLECharacteristic *pVolumetricCharacteristic = nullptr;
    BLECharacteristic *pEventLogCharacteristic = nullptr;

    bool deviceConnected = false;
    bool oldDeviceConnected = false;

    std::queue<std::string> eventLogQueue;
    std::mutex eventQueueMutex;

    TaskHandle_t bleTaskHandle = nullptr;

    static void bleTaskCode(void *parameter);

public:
    BLEManager(State *state, VolumetricsHelper *volumetricsHelper);

    void setup();
    void logEvent(const char *message);
    void updateState();
    void updateVolumetricSettings();
    void processEventQueue();
    void loop();

    class ServerCallbacks : public BLEServerCallbacks
    {
    private:
        BLEManager *_manager;

    public:
        ServerCallbacks(BLEManager *manager) : _manager(manager) {}

        void onConnect(BLEServer *pServer) override
        {
            _manager->deviceConnected = true;
        }

        void onDisconnect(BLEServer *pServer) override
        {
            _manager->deviceConnected = false;
        }
    };
};

#endif // BREWPILOT_BLEMANAGER_H