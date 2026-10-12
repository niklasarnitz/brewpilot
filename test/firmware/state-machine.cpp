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

void simultaneousProgrammingHasOneOwner()
{
    {
        StateMachineFixture competing;
        competing.tick(Event::CONTINUOUS, Event::CONTINUOUS);
        competing.tick(Event::CONTINUOUS_HELD, Event::CONTINUOUS_HELD);
        const auto owner = competing.state.programmingTarget;
        require((owner == ProgrammingTarget::GROUP_ONE && !competing.state.groupOneIsExtracting && competing.state.groupTwoIsExtracting) ||
                    (owner == ProgrammingTarget::GROUP_TWO && competing.state.groupOneIsExtracting && !competing.state.groupTwoIsExtracting),
                "simultaneous holds arm exactly one group while the other remains in normal Continuous");
        competing.tick(Event::CONTINUOUS, Event::CONTINUOUS);
        require(competing.state.programmingTarget == ProgrammingTarget::NONE &&
                    !competing.state.groupOneIsExtracting && !competing.state.groupTwoIsExtracting,
                "both stops cancel the winner's selection and stop the non-owner normally");
    }
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

void programmingIsGroupScoped()
{
    for (int group : {1, 2})
    {
        StateMachineFixture machine;
        machine.volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 9);
        machine.volumes.writeFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO, 15);
        const auto owner = group == 1 ? ProgrammingTarget::GROUP_ONE : ProgrammingTarget::GROUP_TWO;
        machine.selectProgramming(group);
        require(machine.state.programmingTarget == owner,
                "holding Continuous reserves only that group before selecting any dose");
        machine.tick(group == 2 ? Event::LEFT_DOUBLE_ESPRESSO : Event::NONE,
                     group == 1 ? Event::LEFT_DOUBLE_ESPRESSO : Event::NONE);
        require(!machine.state.groupOneIsExtracting && !machine.state.groupTwoIsExtracting &&
                    machine.state.programmingTarget == owner,
                "an unset dose on the other group cannot select or start learning");
        machine.tick(group == 2 ? Event::CONTINUOUS : Event::NONE,
                     group == 1 ? Event::CONTINUOUS : Event::NONE);
        machine.tick(group == 2 ? Event::CONTINUOUS_HELD : Event::NONE,
                     group == 1 ? Event::CONTINUOUS_HELD : Event::NONE);
        require(machine.state.programmingTarget == owner &&
                    (group == 1 ? machine.state.groupTwoIsExtracting : machine.state.groupOneIsExtracting),
                "other group's Continuous remains normal and its hold cannot take ownership");
        machine.tick(group == 2 ? Event::CONTINUOUS : Event::NONE,
                     group == 1 ? Event::CONTINUOUS : Event::NONE);
        require(machine.state.programmingTarget == owner,
                "stopping the other group must not cancel the waiting programming owner");
        machine.tick(group == 1 ? Event::RIGHT_DOUBLE_ESPRESSO : Event::LEFT_SINGLE_ESPRESSO,
                     group == 2 ? Event::RIGHT_DOUBLE_ESPRESSO : Event::LEFT_SINGLE_ESPRESSO);
        machine.pulses(group, 17);
        machine.pulses(3 - group, 9);
        machine.tick();
        require((group == 1 ? machine.state.groupOneIsExtracting : machine.state.groupTwoIsExtracting) &&
                    !(group == 1 ? machine.state.groupTwoIsExtracting : machine.state.groupOneIsExtracting),
                "only the armed group learns beyond its old dose; other group keeps normal dose stop");
        machine.tick(group == 1 ? Event::CONTINUOUS : Event::NONE,
                     group == 2 ? Event::CONTINUOUS : Event::NONE);
        require(machine.state.programmingTarget == ProgrammingTarget::NONE &&
                    machine.volumes.getFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO) == 17 &&
                    machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 9,
                "only the owner's Continuous saves its selected shared dose");
        machine.tick(group == 2 ? Event::RIGHT_DOUBLE_ESPRESSO : Event::NONE,
                     group == 1 ? Event::RIGHT_DOUBLE_ESPRESSO : Event::NONE);
        machine.pulses(3 - group, 16);
        machine.tick();
        require(group == 1 ? machine.state.groupTwoIsExtracting : machine.state.groupOneIsExtracting,
                "other group uses the newly programmed shared pulse value");
        machine.pulses(3 - group, 1);
        machine.tick();
        require(!machine.state.groupOneIsExtracting && !machine.state.groupTwoIsExtracting,
                "shared dose stops at seventeen pulses on the other group");
    }
}

void teaAndGroupProgrammingAreExclusive()
{
    for (int group : {1, 2})
        for (bool learning : {false, true})
        {
            StateMachineFixture machine;
            machine.volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 5);
            machine.volumes.writeTeaWaterSetting(3000);
            machine.selectProgramming(group);
            const auto owner = group == 1 ? ProgrammingTarget::GROUP_ONE : ProgrammingTarget::GROUP_TWO;
            if (learning)
            {
                machine.tick(group == 1 ? Event::LEFT_SINGLE_ESPRESSO : Event::NONE,
                             group == 2 ? Event::LEFT_SINGLE_ESPRESSO : Event::NONE);
                machine.pulses(group, 2);
            }
            machine.tick(Event::NONE, Event::NONE, true);
            machine.tick(Event::NONE, Event::NONE, false, 2000, true);
            require(machine.state.programmingTarget == owner && machine.state.isExtractingTeaWater,
                    "Tea hold cannot steal a waiting or learning group's programming session");
            machine.tick(Event::NONE, Event::NONE, false, 1000);
            require(!machine.state.isExtractingTeaWater && machine.volumes.getTeaWaterSetting() == 3000 &&
                        machine.state.programmingTarget == owner,
                    "Tea remains normally timed while a group owns programming");
            machine.tick(group == 1 ? Event::CONTINUOUS : Event::NONE,
                         group == 2 ? Event::CONTINUOUS : Event::NONE);
            require(machine.state.programmingTarget == ProgrammingTarget::NONE &&
                        machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == (learning ? 2 : 5),
                    "only the group's stop cancels selection or saves its learning dose");
        }

    for (bool learning : {false, true})
    {
        StateMachineFixture machine;
        machine.volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 5);
        machine.volumes.writeTeaWaterSetting(3000);
        machine.tick(Event::NONE, Event::NONE, false, 2000, true);
        if (learning)
            machine.tick(Event::NONE, Event::NONE, true);
        machine.tick(Event::CONTINUOUS, Event::CONTINUOUS);
        machine.tick(Event::CONTINUOUS_HELD, Event::CONTINUOUS_HELD);
        require(machine.state.programmingTarget == ProgrammingTarget::TEA &&
                    machine.state.groupOneIsExtracting && machine.state.groupTwoIsExtracting &&
                    machine.state.isExtractingTeaWater == learning,
                "neither group hold can steal waiting or learning Tea programming");
        machine.tick(Event::CONTINUOUS, Event::CONTINUOUS);
        require(machine.state.programmingTarget == ProgrammingTarget::TEA &&
                    machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 5,
                "normal group stops cannot cancel Tea programming or save a dose");
        if (!learning)
            machine.tick(Event::NONE, Event::NONE, true);
        machine.tick(Event::NONE, Event::NONE, true, 4100);
        require(machine.state.programmingTarget == ProgrammingTarget::NONE &&
                    !machine.state.isExtractingTeaWater &&
                    machine.volumes.getTeaWaterSetting() == (learning ? 4250 : 4100),
                "Tea can save and release ownership after competing group holds");
        machine.selectProgramming(2);
        machine.tick(Event::NONE, Event::CONTINUOUS);
        require(machine.state.programmingTarget == ProgrammingTarget::NONE,
                "a completed Tea session does not block later group programming");
    }
}

int main()
{
    try
    {
        programmingIsGroupScoped();
        require(runFirmwareRegressionTests() == 0, "firmware regression suite failed");
        require(runGroupStateTests() == 0, "group state suite failed");
        require(runBoilerTeaStateTests() == 0, "boiler/tea state suite failed");
        runButtonEventTests();
        std::cout << "PASS: button Press/Held/Release boundaries, all dose events, chords and STOP priorities\n";
        runInputOutputTests();
        outputStateMatrix();
        simultaneousProgrammingHasOneOwner();
        teaAndGroupProgrammingAreExclusive();
        std::cout << "PASS: output matrix and programming arbitration\n";
    }
    catch (const std::exception &error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
