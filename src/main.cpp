#include <Arduino.h>
#include "structs/ButtonEvent.h"
#include "StaticConfig.h"
#include "hardware/state-handlers/StateHandler.h"
#include "hardware/state/State.h"
#include "hardware/actors/StateActor.h"
#include "hardware/handlers/InputHandler.h"

ButtonEvent buttonEvent{};
State state{};
StateHandler stateHandler(&state, &buttonEvent);
StateActor stateActor(&state);
InputHandler inputHandler(&buttonEvent);

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

    pinMode(GROUP_ONE_FLOW_METER_PIN, INPUT_PULLDOWN);
    pinMode(GROUP_TWO_FLOW_METER_PIN, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(GROUP_ONE_FLOW_METER_PIN), groupOneFlowMeterHandler, RISING);
    attachInterrupt(digitalPinToInterrupt(GROUP_TWO_FLOW_METER_PIN), groupTwoFlowMeterHandler, RISING);
}

void loop()
{
    inputHandler.readInputs();
    stateHandler.handleState();
    stateActor.loop();

    delay(50);
}