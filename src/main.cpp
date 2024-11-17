#include <Arduino.h>
#include "structs/ButtonEvent.h"
#include "Config.h"
#include "hardware/state-handlers/StateHandler.h"
#include "hardware/state/State.h"
#include "hardware/actors/StateActor.h"
#include "hardware/handlers/InputHandler.h"

ButtonEvent buttonEvent;
State state;
StateHandler stateHandler(state, buttonEvent);
StateActor stateActor(state);
InputHandler inputHandler(buttonEvent);

void groupOneFlowMeterHandler() {
    stateHandler.groupOneFlowMeterPulseInterrupt();
}

void groupTwoFlowMeterHandler() {
    stateHandler.groupTwoFlowMeterPulseInterrupt();
}

void setup() {
    pinMode(GROUP_ONE_FLOW_METER_PIN, INPUT);
    pinMode(GROUP_TWO_FLOW_METER_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(GROUP_ONE_FLOW_METER_PIN), groupOneFlowMeterHandler, CHANGE);
    attachInterrupt(digitalPinToInterrupt(GROUP_TWO_FLOW_METER_PIN), groupTwoFlowMeterHandler, CHANGE);
}

void loop() {
    inputHandler.readInputs();
    stateHandler.handleState();
    stateActor.loop();
    delay(10);
}