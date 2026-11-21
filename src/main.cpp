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

PreferenceHelper preferenceHelper;
VolumetricsHelper volumetricsHelper(&preferenceHelper);
ButtonEvent buttonEvent{};
State state{};
StateHandler stateHandler(&state, &buttonEvent, &volumetricsHelper);
StateActor stateActor(&state);
InputHandler inputHandler(&buttonEvent, &state.isInProgrammingMode);

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

    // R9 / R8 provide external 10k pull-ups for the flow meters.
    pinMode(GROUP_ONE_FLOW_METER_PIN, INPUT);
    pinMode(GROUP_TWO_FLOW_METER_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(GROUP_ONE_FLOW_METER_PIN), groupOneFlowMeterHandler, FALLING);
    attachInterrupt(digitalPinToInterrupt(GROUP_TWO_FLOW_METER_PIN), groupTwoFlowMeterHandler, FALLING);

    // Load Volumetric Settings
    volumetricsHelper.setup();
}

void loop()
{
    inputHandler.readInputs();
    stateHandler.handleState();
    stateActor.loop();

    delay(50);
}
