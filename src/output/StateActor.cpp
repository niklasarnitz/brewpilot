#include "output/StateActor.h"

#include "config/BoardPins.h"
#include "config/SelectedMachine.h"

StateActor::StateActor(const State &state)
    : state(state), pumpRelay(pins::RELAY_PUMP, "Pump", machineConfig.relayConfig.pumpInverted),
      boilerFillSolenoid(pins::RELAY_BOILER_FILL, "Boiler Fill", machineConfig.relayConfig.boilerFillInverted),
#if BREWPILOT_GROUP_ONE_ACTIVE
      groupOneSolenoid(pins::RELAY_GROUP_ONE, "Group One", machineConfig.relayConfig.groupOneInverted),
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
      groupTwoSolenoid(pins::RELAY_GROUP_TWO, "Group Two", machineConfig.relayConfig.groupTwoInverted),
#endif
      teaWaterSolenoid(pins::RELAY_TEA, "Tea Water Cold Water", machineConfig.relayConfig.teaInverted) {}

void StateActor::loop()
{
    // Boiler
    boilerFillSolenoid.setOpen(state.isFillingBoiler);

    // Group Heads
    bool groupIsExtracting = false;
#if BREWPILOT_GROUP_ONE_ACTIVE
    groupOneSolenoid.setOpen(state.groupOneIsExtracting);
    groupIsExtracting |= state.groupOneIsExtracting;
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
    groupTwoSolenoid.setOpen(state.groupTwoIsExtracting);
    groupIsExtracting |= state.groupTwoIsExtracting;
#endif

    // Tea Water
    teaWaterSolenoid.setOpen(state.isExtractingTeaWater);

    // Pump
    pumpRelay.setEnabled(state.isFillingBoiler || groupIsExtracting);
}
