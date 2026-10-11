#include "StateMachineFixture.h"

// Firmware headers define non-inline functions (e.g. getMachineConfig), so
// build the suites as one translation unit, just like the Arduino firmware.
#include "main.cpp"
#include "group-state.cpp"
#include "boiler-tea-state.cpp"
#include "button-events.cpp"
#include "input-output.cpp"

void outputStateMatrix()
{
    StateMachineFixture machine;
    for (unsigned mask = 0; mask < 16; ++mask)
    {
        machine.state.isFillingBoiler = mask & 1;
        machine.state.groupOneIsExtracting = mask & 2;
        machine.state.groupTwoIsExtracting = mask & 4;
        machine.state.isExtractingTeaWater = mask & 8;
        machine.actor.loop();
        require(testPins[RELAY_BOILER_FILL] == ((bool(mask & 1) != config.relayConfig.boilerFillInverted) ? HIGH : LOW),
                "boiler valve must follow only boiler fill state");
        require(testPins[RELAY_GROUP_ONE] == ((bool(mask & 2) != config.relayConfig.groupOneInverted) ? HIGH : LOW),
                "group one valve must follow only group one state");
        require(testPins[RELAY_GROUP_TWO] == ((bool(mask & 4) != config.relayConfig.groupTwoInverted) ? HIGH : LOW),
                "group two valve must follow only group two state");
        require(testPins[RELAY_TEA] == ((bool(mask & 8) != config.relayConfig.teaInverted) ? HIGH : LOW),
                "tea valve must follow only tea state");
        require(testPins[RELAY_PUMP] == ((bool(mask & 7) != config.relayConfig.pumpInverted) ? HIGH : LOW),
                "pump must serve either group or boiler, but not tea alone");
        machine.actor.loop(); // Stable output must also remain correct without a transition.
        require(testPins[RELAY_PUMP] == ((bool(mask & 7) != config.relayConfig.pumpInverted) ? HIGH : LOW),
                "repeated actor loop must preserve pump command");
    }
}

void requireLocked(const StateMachineFixture &machine)
{
    require(machine.state.boilerFillFault && !machine.state.isFillingBoiler &&
                !machine.state.groupOneIsExtracting && !machine.state.groupTwoIsExtracting &&
                !machine.state.isExtractingTeaWater && machine.state.programmingTarget == ProgrammingTarget::NONE,
            "latched fill fault must lock every water operation and cancel programming");
}

void faultAcknowledgementIsGroupSpecific()
{
    for (int group : {1, 2})
    {
        StateMachineFixture machine;
        machine.volumes.writeTeaWaterSetting(5000);
        testProbe = 1001;
        machine.tick();
        machine.tick();
        machine.selectProgramming(group);
        machine.tick(Event::AUTO_BACKFLUSH, Event::AUTO_BACKFLUSH, true);
        machine.tick(Event::CONTINUOUS, Event::CONTINUOUS, true, 120000);
        requireLocked(machine); // Presses on the fault transition must not arm acknowledgement.
        machine.tick(Event::CONTINUOUS_HELD, Event::CONTINUOUS_HELD);
        requireLocked(machine);
        machine.tick(group == 1 ? Event::CONTINUOUS : Event::NONE,
                     group == 2 ? Event::CONTINUOUS : Event::NONE);
        requireLocked(machine);
        machine.tick(group == 2 ? Event::CONTINUOUS_HELD : Event::NONE,
                     group == 1 ? Event::CONTINUOUS_HELD : Event::NONE);
        requireLocked(machine); // The other group's held event cannot use this group's press.
        machine.tick(group == 1 ? Event::CONTINUOUS_HELD : Event::NONE,
                     group == 2 ? Event::CONTINUOUS_HELD : Event::NONE, true);
        require(!machine.state.boilerFillFault && !machine.state.isFillingBoiler &&
                    !machine.state.groupOneIsExtracting && !machine.state.groupTwoIsExtracting &&
                    !machine.state.isExtractingTeaWater && machine.state.programmingTarget == ProgrammingTarget::NONE,
                "acknowledgement must be consumed without starting water or programming");
        machine.tick();
        require(!machine.state.isFillingBoiler, "acknowledged probe must settle before filling again");
        machine.tick();
        require(machine.state.isFillingBoiler, "acknowledgement must reset boiler sampling and allow a fresh fill");
        machine.tick(Event::NONE, Event::NONE, false, 120000);
        requireLocked(machine);
        machine.tick(Event::CONTINUOUS_HELD, Event::CONTINUOUS_HELD);
        requireLocked(machine); // Previous acknowledgement must not remain armed.
    }
}

void simultaneousProgrammingHasOneOwner()
{
    StateMachineFixture machine;
    machine.volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 9);
    machine.volumes.writeFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO, 15);
    machine.volumes.writeTeaWaterSetting(3000);
    machine.selectProgramming();
    machine.tick(Event::LEFT_SINGLE_ESPRESSO, Event::RIGHT_DOUBLE_ESPRESSO, true);
    require(machine.state.programmingTarget == ProgrammingTarget::GROUP_ONE,
            "simultaneous selection must have exactly one owner in handler order");
    machine.pulses(1, 11);
    machine.pulses(2, 15);
    machine.tick();
    require(machine.state.groupOneIsExtracting && !machine.state.groupTwoIsExtracting,
            "only the programming owner may bypass its normal dose");
    machine.tick(Event::CONTINUOUS, Event::NONE, false, 3000);
    require(machine.state.programmingTarget == ProgrammingTarget::NONE && !machine.state.isExtractingTeaWater,
            "owner stop must leave programming and unrelated tea must keep its timer");
    require(machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 11 &&
                machine.volumes.getFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO) == 15 &&
                machine.volumes.getTeaWaterSetting() == 3000,
            "simultaneous drinks must not overwrite each other's settings");
}

int main()
{
    try
    {
        require(runFirmwareRegressionTests() == 0, "firmware regression suite failed");
        require(runGroupStateTests() == 0, "group state suite failed");
        require(runBoilerTeaStateTests() == 0, "boiler/tea state suite failed");
        runButtonEventTests();
        std::cout << "PASS: button Press/Held/Release boundaries, all dose events, chords and STOP priorities\n";
        runInputOutputTests();
        outputStateMatrix();
        faultAcknowledgementIsGroupSpecific();
        simultaneousProgrammingHasOneOwner();
        std::cout << "PASS: output matrix, group-specific fault acknowledgement, programming arbitration\n";
    }
    catch (const std::exception &error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
