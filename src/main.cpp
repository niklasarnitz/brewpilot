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
#include "utils/DeviceNameHelper.h"
#include "hardware/ble/BLECoreManager.h"

PreferenceHelper preferenceHelper;
VolumetricsHelper volumetricsHelper(&preferenceHelper);
DeviceNameHelper deviceNameHelper(&preferenceHelper);
ButtonEvent buttonEvent{};
State state{};
StateHandler stateHandler(&state, &buttonEvent, &volumetricsHelper);
StateActor stateActor(&state);
InputHandler inputHandler(&buttonEvent, &state.isInProgrammingMode);
BLECoreManager bleCoreManager(&state, &preferenceHelper, &volumetricsHelper, &deviceNameHelper);

void IRAM_ATTR groupOneFlowMeterHandler()
{
    stateHandler.groupOneFlowMeterPulseInterrupt();
}

void IRAM_ATTR groupTwoFlowMeterHandler()
{
    stateHandler.groupTwoFlowMeterPulseInterrupt();
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

    // Initialize device name (generates random suffix if needed)
    deviceNameHelper.initialize();

    bleCoreManager.begin();
}

void loop()
{
    inputHandler.readInputs();
    stateHandler.handleState();
    stateActor.loop();
    // NimBLE is non blocking so it can run in the main loop
    bleCoreManager.loop();

    delay(50);
}