#include "StateMachineFixture.h"
#include "Scenarios.h"
#include "input/GroupHeadButtonHandler.h"

namespace
{
struct Machine : StateMachineFixture
{
    Machine()
    {
        volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 7);
        volumes.writeFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO, 13);
        volumes.writeTeaWaterSetting(3000);
    }
};
} // namespace

void failedStoragePreservesSettings()
{
    Machine machine;
    for (bool failOpen : {true, false})
    {
        testFailOpen = failOpen;
        testFailWrite = !failOpen;
        require(!machine.volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 19),
                "dose write failure must be reported to its caller");
        require(!machine.volumes.writeTeaWaterSetting(7000), "tea write failure must be reported to its caller");
        require(machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 7,
                "failed NVS write must preserve the active dose");
        require(machine.volumes.getTeaWaterSetting() == 3000,
                "failed NVS write must preserve the active tea duration");
    }
    testFailOpen = testFailWrite = false;
    require(!machine.volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 0), "zero pulses must not erase a dose");
    require(!machine.volumes.writeTeaWaterSetting(0), "zero duration must not erase tea setting");
    require(machine.volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 19), "valid dose must save");
    require(machine.volumes.writeTeaWaterSetting(7000), "valid tea duration must save");
    PreferenceHelper rebootPreferences;
    VolumetricsHelper rebootVolumes{rebootPreferences};
    rebootVolumes.setup();
    require(rebootVolumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 19,
            "successful dose must survive reboot");
    require(rebootVolumes.getTeaWaterSetting() == 7000, "successful tea duration must survive reboot");
}

void programmingDoesNotCaptureOtherDrinks()
{
    {
        Machine machine;
        machine.tick(Event::NONE, Event::LEFT_SINGLE_ESPRESSO);
        machine.selectProgramming();
        machine.pulses(2, 3);
        machine.tick(Event::NONE, Event::CONTINUOUS);
        machine.tick(Event::RIGHT_DOUBLE_ESPRESSO);
        machine.pulses(1, 17);
        machine.tick();
        require(machine.state.groupOneIsExtracting, "stopping an existing drink must not cancel programming selection");
        machine.tick(Event::CONTINUOUS);
        require(machine.volumes.getFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO) == 17,
                "selection must survive an unrelated stop");
    }
    Machine machine;
    machine.tick(Event::NONE, Event::LEFT_SINGLE_ESPRESSO, true);
    machine.selectProgramming();
    machine.pulses(2, 7);
    machine.tick(Event::NONE, Event::NONE, false, 1000);
    require(!machine.state.groupTwoIsExtracting, "other group must stop at its dose while selecting programming");
    require(!machine.state.isExtractingTeaWater, "existing tea must retain its time limit while selecting programming");

    machine.tick(Event::RIGHT_DOUBLE_ESPRESSO, Event::LEFT_SINGLE_ESPRESSO, true);
    machine.pulses(1, 17);
    machine.pulses(2, 3);
    machine.tick(Event::NONE, Event::CONTINUOUS);
    require(machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 7,
            "stopping another group must not overwrite its dose");
    machine.tick(Event::NONE, Event::NONE, false, 3000);
    require(machine.state.groupOneIsExtracting, "learning group must stay active beyond its old pulse target");
    require(!machine.state.isExtractingTeaWater, "tea must retain its limit during group learning");
    machine.tick(Event::CONTINUOUS);
    require(machine.volumes.getFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO) == 17,
            "only the selected dose must be learned");

    machine.selectProgramming(2);
    machine.tick(Event::NONE, Event::LEFT_SINGLE_ESPRESSO);
    machine.pulses(2, 11);
    machine.tick();
    require(machine.state.groupTwoIsExtracting, "group two can own a learning extraction");
    machine.tick(Event::NONE, Event::CONTINUOUS);
    require(machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 11,
            "group two must save its own learned dose");

    machine.tick(Event::NONE, Event::NONE, false, 2000, true);
    machine.tick(Event::NONE, Event::NONE, true);
    machine.tick(Event::RIGHT_DOUBLE_ESPRESSO);
    machine.pulses(1, 17);
    machine.tick(Event::NONE, Event::NONE, false, 4000);
    require(!machine.state.groupOneIsExtracting, "group dose must finish while tea is learning");
    require(machine.state.isExtractingTeaWater, "learning tea must outlast its old duration");
    machine.tick(Event::NONE, Event::NONE, true, 50);
    require(machine.volumes.getTeaWaterSetting() == 4100, "tea must save only its own duration");
}

void programmingCancelPreservesLastDose()
{
    Machine machine;
    machine.tick(Event::LEFT_SINGLE_ESPRESSO);
    machine.pulses(1, 7);
    machine.tick();
    machine.selectProgramming();
    machine.tick(Event::CONTINUOUS);
    require(!machine.state.groupOneIsExtracting, "cancel before dose selection must not start water");
    machine.tick(Event::CONTINUOUS);
    machine.pulses(1, 3);
    machine.tick(Event::CONTINUOUS);
    require(machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 7,
            "canceling selection must not retain a stale programming target");
}

void stopWinsOverBackflush()
{
    Machine machine;
    GroupButtonMatrixState keys{};
    Event event = Event::NONE;
    GroupHeadButtonHandler buttons(keys, event, 1);
    machine.tick(Event::AUTO_BACKFLUSH);
    keys.leftSingleDose = keys.rightDoubleDose = keys.continuous = true;
    buttons.handle();
    machine.tick(event);
    require(!machine.state.groupOneIsExtracting, "simultaneous STOP must cancel backflush");
    buttons.handle();
    machine.tick(event);
    require(!machine.state.groupOneIsExtracting, "held keys must not restart canceled backflush");
    keys.leftSingleDose = keys.rightDoubleDose = false;
    buttons.handle();
    keys.leftSingleDose = keys.rightDoubleDose = true;
    buttons.handle();
    machine.tick(event);
    require(!machine.state.groupOneIsExtracting, "combo must not restart water while STOP remains held");
}

void normalDosesAndBoilerStillStop()
{
    Machine machine;
    machine.pulses(1, 20); // Idle flow must not count toward the next extraction.
    machine.tick(Event::LEFT_SINGLE_ESPRESSO, Event::RIGHT_DOUBLE_ESPRESSO, true);
    machine.pulses(1, 6);
    machine.pulses(2, 12);
    machine.tick();
    require(machine.state.groupOneIsExtracting && machine.state.groupTwoIsExtracting,
            "both doses must remain active one pulse below their targets");
    machine.pulses(1, 1);
    machine.tick();
    require(!machine.state.groupOneIsExtracting && machine.state.groupTwoIsExtracting,
            "groups must stop independently at their exact pulse targets");
    require((testPins[pins::RELAY_PUMP] == HIGH) != machineConfig.relayConfig.pumpInverted,
            "one finished group must not stop the other group's pump");
    machine.pulses(2, 1);
    machine.tick(Event::NONE, Event::NONE, false, 2899);
    require(!machine.state.groupTwoIsExtracting && machine.state.isExtractingTeaWater,
            "tea must still run at 2999 milliseconds");
    machine.tick(Event::NONE, Event::NONE, false, 1);
    require(!machine.state.isExtractingTeaWater, "normal tea must stop at its exact duration");

    testProbe = 3000;
    for (int i = 0; i < 50; ++i)
        machine.tick();
    require(machine.state.isFillingBoiler, "dry boiler must fill normally");
    testProbe = 0;
    for (int i = 0; i < 100; ++i)
        machine.tick();
    require(!machine.state.isFillingBoiler, "full boiler must stop normally");
}

void extractionHasNoDeadline()
{
    for (uint32_t start : {1u, 0xfffff000u})
        for (Event mode : {Event::NONE, Event::CONTINUOUS, Event::LEFT_SINGLE_ESPRESSO})
        {
            Machine machine;
            testMillis = start;
            const bool learning = mode == Event::NONE;
            if (learning)
                machine.selectProgramming();
            machine.tick(learning ? Event::LEFT_SINGLE_ESPRESSO : mode);
            const uint32_t began = testMillis;
            for (uint32_t elapsed : {9999u, 10000u, 10001u, 119999u, 120000u, 120001u, 240000u})
            {
                testMillis = uint32_t(began + elapsed);
                machine.tick(Event::NONE, Event::NONE, false, 0);
                require(machine.state.groupOneIsExtracting, "dose, continuous and learning have no elapsed-time cutoff");
                require((testPins[pins::RELAY_GROUP_ONE] == HIGH) != machineConfig.relayConfig.groupOneInverted &&
                            (testPins[pins::RELAY_PUMP] == HIGH) != machineConfig.relayConfig.pumpInverted,
                        "prolonged extraction keeps the group valve and pump active");
            }
            machine.pulses(1, 3);
            machine.tick(Event::CONTINUOUS, Event::NONE, false, 0);
            require(!machine.state.groupOneIsExtracting &&
                        machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == (learning ? 3 : 7),
                    "manual stop remains effective and only learning saves after prolonged extraction");
        }
}

void backflushRemainsBoundedWithoutFlow()
{
    Machine machine;
    machine.tick(Event::AUTO_BACKFLUSH);
    for (int cycle = 0; cycle < 5; ++cycle)
    {
        require(machine.state.groupOneIsExtracting, "each backflush extraction must start");
        machine.tick(Event::NONE, Event::NONE, false, 9999);
        require(machine.state.groupOneIsExtracting, "backflush must last ten seconds without flow");
        machine.tick(Event::NONE, Event::NONE, false, 1);
        require(!machine.state.groupOneIsExtracting, "backflush must stop after ten seconds");
        if (cycle < 4)
            machine.tick(Event::NONE, Event::NONE, false, 5000);
    }
    machine.tick(Event::NONE, Event::NONE, false, 5000);
    require(!machine.state.groupOneIsExtracting, "backflush must finish after five cycles");
}
