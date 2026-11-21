#include <Arduino.h>

#include "config/BoardPins.h"
#include "config/SelectedMachine.h"
#include "input/ButtonEvents.h"
#include "input/InputHandler.h"
#include "output/StateActor.h"
#include "state/State.h"
#include "state/StateHandler.h"
#include "storage/PreferenceHelper.h"
#include "storage/VolumetricsHelper.h"

PreferenceHelper preferenceHelper;
VolumetricsHelper volumetricsHelper(preferenceHelper);
ButtonEvent buttonEvent{};
State state{};
StateHandler stateHandler(state, buttonEvent, volumetricsHelper);
StateActor stateActor(state);
InputHandler inputHandler(buttonEvent);

#if BREWPILOT_GROUP_ONE_ACTIVE
void IRAM_ATTR groupOneFlowMeterHandler()
{
    stateHandler.groupOneFlowMeterPulseInterrupt();
}
#endif

#if BREWPILOT_GROUP_TWO_ACTIVE
void IRAM_ATTR groupTwoFlowMeterHandler()
{
    stateHandler.groupTwoFlowMeterPulseInterrupt();
}
#endif

void setup()
{
    Serial.begin(115200);

    // R9 / R8 provide external 10k pull-ups for the flow meters.
#if BREWPILOT_GROUP_ONE_ACTIVE
    pinMode(pins::GROUP_ONE_FLOW_METER, INPUT);
#if BREWPILOT_HAS_FLOWMETERS
    attachInterrupt(digitalPinToInterrupt(pins::GROUP_ONE_FLOW_METER), groupOneFlowMeterHandler, FALLING);
#endif
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
    pinMode(pins::GROUP_TWO_FLOW_METER, INPUT);
#if BREWPILOT_HAS_FLOWMETERS
    attachInterrupt(digitalPinToInterrupt(pins::GROUP_TWO_FLOW_METER), groupTwoFlowMeterHandler, FALLING);
#endif
#endif

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
