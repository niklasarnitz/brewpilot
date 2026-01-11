#include "StateActor.h"

StateActor::StateActor(State *state)
    : state(state), pumpRelay(RELAY_PUMP, "Pump", config.relayConfig.pumpInverted),
      boilerFillSolenoid(RELAY_BOILER_FILL, "Boiler Fill", config.relayConfig.boilerFillInverted),
      groupOneSolenoid(RELAY_GROUP_ONE, "Group One", config.relayConfig.groupOneInverted),
      groupTwoSolenoid(RELAY_GROUP_TWO, "Group Two", config.relayConfig.groupTwoInverted),
      teaWaterSolenoid(RELAY_TEA, "Tea Water Cold Water", config.relayConfig.teaInverted) {};

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