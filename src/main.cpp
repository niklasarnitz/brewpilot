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
#include "hardware/scales/ScaleHandler.h"

PreferenceHelper preferenceHelper;
VolumetricsHelper volumetricsHelper(&preferenceHelper);
ButtonEvent buttonEvent{};
State state{};
StateHandler stateHandler(&state, &buttonEvent, &volumetricsHelper);
StateActor stateActor(&state);
InputHandler inputHandler(&buttonEvent, &state.isInProgrammingMode);
ScaleHandler scaleHandler{};

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
    BLE.begin();
    BLE.setLocalName("BrewPilot");

    pinMode(GROUP_ONE_FLOW_METER_PIN, INPUT_PULLUP);
    // TODO: has external pullup
    pinMode(GROUP_TWO_FLOW_METER_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(GROUP_ONE_FLOW_METER_PIN), groupOneFlowMeterHandler, FALLING);
    attachInterrupt(digitalPinToInterrupt(GROUP_TWO_FLOW_METER_PIN), groupTwoFlowMeterHandler, FALLING);

    // Load Volumetric Settings
    volumetricsHelper.setup();
}

void loop()
{
    scaleHandler.handleScale();
    inputHandler.readInputs();
    stateHandler.handleState();
    stateActor.loop();

    delay(50);
}