#include <Arduino.h>

#include "structs/ButtonEvent.h"
#include "StaticConfig.h"
#include "hardware/state-handlers/StateHandler.h"
#include "hardware/state/State.h"
#include "hardware/actors/StateActor.h"
#include "hardware/handlers/InputHandler.h"
#include "structs/VolumetricSettings.h"
#include "utils/PreferenceHelper.h"
#include "utils/VolumetricsHelper.h"
#include "hardware/ble/BLECoreManager.h"

PreferenceHelper preferenceHelper;
VolumetricsHelper volumetricsHelper(&preferenceHelper);
ButtonEvent buttonEvent{};
State state{};
StateHandler stateHandler(&state, &buttonEvent, &volumetricsHelper);
StateActor stateActor(&state);
InputHandler inputHandler(&buttonEvent, &state.isInProgrammingMode);
BLECoreManager bleCoreManager(&state, &preferenceHelper);

void IRAM_ATTR groupOneFlowMeterHandler()
{
    stateHandler.groupOneFlowMeterPulseInterrupt();
}

void IRAM_ATTR groupTwoFlowMeterHandler()
{
    stateHandler.groupTwoFlowMeterPulseInterrupt();
}

// Task handle for BLE core
TaskHandle_t bleTaskHandle = nullptr;

// BLE core task (runs on core 1)
void bleCore1Task(void *pvParameters)
{
    BLECoreManager *pBleManager = (BLECoreManager *)pvParameters;
    
    // Initialize BLE on core 1
    pBleManager->begin("BrewPilot");
    
    // Main loop for BLE
    while (true)
    {
        pBleManager->loop();
        delay(100);
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(GROUP_ONE_FLOW_METER_PIN, INPUT_PULLUP);
    // TODO: has external pullup
    pinMode(GROUP_TWO_FLOW_METER_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(GROUP_ONE_FLOW_METER_PIN), groupOneFlowMeterHandler, FALLING);
    attachInterrupt(digitalPinToInterrupt(GROUP_TWO_FLOW_METER_PIN), groupTwoFlowMeterHandler, FALLING);

    // Load Volumetric Settings
    volumetricsHelper.setup();
    
    // Start BLE on core 1
    xTaskCreatePinnedToCore(
        bleCore1Task,           // Task function
        "BLECore1",             // Task name
        4096,                   // Stack size
        &bleCoreManager,        // Parameters
        1,                      // Priority
        &bleTaskHandle,         // Task handle
        1                       // Core ID (1 = second core)
    );
}

void loop()
{
    inputHandler.readInputs();
    stateHandler.handleState();
    stateActor.loop();

    delay(50);
}