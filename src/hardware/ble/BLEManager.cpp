#include "BLEManager.h"
#include <ArduinoJson.h>

BLEManager::BLEManager(State *state, VolumetricsHelper *volumetricsHelper) : _state(state), _volumetricsHelper(volumetricsHelper)
{
}

void BLEManager::bleTaskCode(void *parameter)
{
    BLEManager *manager = static_cast<BLEManager *>(parameter);

    while (true)
    {
        manager->loop();
        delay(100); // Run BLE updates at 10Hz
    }
}

void BLEManager::setup()
{
    // Initialize BLE
    BLEDevice::init("BrewPilot");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this));

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create BLE Characteristics
    pStateCharacteristic = pService->createCharacteristic(
        STATE_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY);
    pStateCharacteristic->addDescriptor(new BLE2902());

    pVolumetricCharacteristic = pService->createCharacteristic(
        VOLUMETRIC_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY);
    pVolumetricCharacteristic->addDescriptor(new BLE2902());

    pEventLogCharacteristic = pService->createCharacteristic(
        EVENT_LOG_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY);
    pEventLogCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("BLE service started. Advertising...");

    // Create a task that will handle BLE operations on Core 0
    xTaskCreatePinnedToCore(
        bleTaskCode,    // Task function
        "BLETask",      // Task name
        10000,          // Stack size (bytes)
        this,           // Parameter
        1,              // Task priority
        &bleTaskHandle, // Task handle
        0               // Core where the task should run (0 = Core 0, Arduino uses Core 1)
    );
}

void BLEManager::logEvent(const char *message)
{
    std::lock_guard<std::mutex> lock(eventQueueMutex);

    // Add to queue, removing oldest if queue is full
    if (eventLogQueue.size() >= 10)
    {
        eventLogQueue.pop();
    }

    eventLogQueue.push(std::string(message));
}

void BLEManager::updateState()
{
    if (deviceConnected)
    {
        JsonDocument doc;

        doc["isFillingBoiler"] = _state->isFillingBoiler;
        doc["isExtractingTeaWater"] = _state->isExtractingTeaWater;
        doc["groupOneIsExtracting"] = _state->groupOneIsExtracting;
        doc["groupTwoIsExtracting"] = _state->groupTwoIsExtracting;
        doc["isInProgrammingMode"] = _state->isInProgrammingMode;

        String jsonString;
        serializeJson(doc, jsonString);

        pStateCharacteristic->setValue(jsonString.c_str());
        pStateCharacteristic->notify();
    }
}

void BLEManager::updateVolumetricSettings()
{
    if (deviceConnected)
    {
        JsonDocument doc;

        VolumetricSettings *volumetricSettings = _volumetricsHelper->getVolumetricSettings();

        doc["leftSingleEspressoPulses"] = volumetricSettings->leftSingleEspressoPulses;
        doc["leftDoubleEspressoPulses"] = volumetricSettings->leftDoubleEspressoPulses;
        doc["rightSingleEspressoPulses"] = volumetricSettings->rightSingleEspressoPulses;
        doc["rightDoubleEspressoPulses"] = volumetricSettings->rightDoubleEspressoPulses;
        doc["teaWaterMilliseconds"] = volumetricSettings->teaWaterMilliseconds;

        String jsonString;
        serializeJson(doc, jsonString);

        pVolumetricCharacteristic->setValue(jsonString.c_str());
        pVolumetricCharacteristic->notify();
    }
}

void BLEManager::processEventQueue()
{
    if (deviceConnected)
    {
        std::lock_guard<std::mutex> lock(eventQueueMutex);

        if (!eventLogQueue.empty())
        {
            std::string event = eventLogQueue.front();
            eventLogQueue.pop();

            pEventLogCharacteristic->setValue(event.c_str());
            pEventLogCharacteristic->notify();
        }
    }
}

void BLEManager::loop()
{
    // Disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // Give time for the client to receive the disconnect event
        pServer->startAdvertising(); // Restart advertising
        Serial.println("BLE client disconnected. Started advertising");
        oldDeviceConnected = deviceConnected;
    }

    // Connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        oldDeviceConnected = deviceConnected;
        Serial.println("BLE client connected");

        // Send initial state and volumetric settings
        updateState();
        updateVolumetricSettings();
    }

    if (deviceConnected)
    {
        // Update characteristic values
        updateState();
        updateVolumetricSettings();
        processEventQueue();
    }
}