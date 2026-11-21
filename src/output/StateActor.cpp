#include "output/StateActor.h"

#include "config/BoardPins.h"
#include "config/SelectedMachine.h"

StateActor::StateActor(const State &state)
    : state(state), pumpRelay(pins::RELAY_PUMP, "Pump", machineConfig.relayConfig.pumpInverted),
      boilerFillSolenoid(pins::RELAY_BOILER_FILL, "Boiler Fill", machineConfig.relayConfig.boilerFillInverted),
      groupOneSolenoid(pins::RELAY_GROUP_ONE, "Group One", machineConfig.relayConfig.groupOneInverted),
      groupTwoSolenoid(pins::RELAY_GROUP_TWO, "Group Two", machineConfig.relayConfig.groupTwoInverted),
      teaWaterSolenoid(pins::RELAY_TEA, "Tea Water Cold Water", machineConfig.relayConfig.teaInverted) {}

void StateActor::loop()
{
    // Boiler
    boilerFillSolenoid.setOpen(state.isFillingBoiler);

    // Group Heads
    bool groupIsExtracting = false;
    if constexpr (GROUP_ONE_ACTIVE)
    {
        groupOneSolenoid.setOpen(state.groupOneIsExtracting);
        groupIsExtracting |= state.groupOneIsExtracting;
    }
    if constexpr (GROUP_TWO_ACTIVE)
    {
        groupTwoSolenoid.setOpen(state.groupTwoIsExtracting);
        groupIsExtracting |= state.groupTwoIsExtracting;
    }

    // Tea Water
    teaWaterSolenoid.setOpen(state.isExtractingTeaWater);

    // Pump
    pumpRelay.setEnabled(state.isFillingBoiler || groupIsExtracting);
}
