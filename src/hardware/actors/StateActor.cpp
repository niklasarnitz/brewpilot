#include "StateActor.h"

StateActor::StateActor(State *state)
    : state(state) {}

void StateActor::loop()
{
    // Boiler
    boilerFillSolenoid.setOpen(state->isFillingBoiler);

    // Group Heads
    groupOneSolenoid.setOpen(state->groupOneIsExtracting);
    groupTwoSolenoid.setOpen(state->groupTwoIsExtracting);

    // Tea Water
    teaWaterSolenoid.setOpen(state->isExtractingTeaWater);

    // Pump
    pumpRelay.setEnabled(state->isFillingBoiler || state->groupOneIsExtracting || state->groupTwoIsExtracting);
}