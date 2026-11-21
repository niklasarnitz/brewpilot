#include "output/StateActor.h"

#include "config/BoardPins.h"
#include "config/SelectedMachine.h"

StateActor::StateActor(State *state)
    : state(state), pumpRelay(pins::RELAY_PUMP, "Pump", machineConfig.relayConfig.pumpInverted),
      boilerFillSolenoid(pins::RELAY_BOILER_FILL, "Boiler Fill", machineConfig.relayConfig.boilerFillInverted),
      groupOneSolenoid(pins::RELAY_GROUP_ONE, "Group One", machineConfig.relayConfig.groupOneInverted),
      groupTwoSolenoid(pins::RELAY_GROUP_TWO, "Group Two", machineConfig.relayConfig.groupTwoInverted),
      teaWaterSolenoid(pins::RELAY_TEA, "Tea Water Cold Water", machineConfig.relayConfig.teaInverted) {}

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
