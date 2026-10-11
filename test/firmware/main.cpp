#include <iostream>
#include <stdexcept>
#include "structs/ButtonEvent.h"
#include "hardware/state-handlers/StateHandler.h"
#include "hardware/handlers/GroupHeadButtonHandler.h"
#include "hardware/actors/StateActor.h"

using Event = GroupHeadButtonEvent;

void require(bool condition, const char *message)
{
    if (!condition)
        throw std::runtime_error(message);
}

struct Machine
{
    PreferenceHelper preferences;
    VolumetricsHelper volumes{&preferences};
    State state{};
    ButtonEvent events{};
    StateHandler handler{&state, &events, &volumes};
    StateActor actor{&state};

    Machine()
    {
        testStorage.clear();
        testFailOpen = testFailWrite = false;
        testMillis = 1;
        testProbe = 0;
        volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 7);
        volumes.writeFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO, 13);
        volumes.writeTeaWaterSetting(3000);
    }

    void tick(Event one = Event::NONE, Event two = Event::NONE, bool tea = false, uint32_t elapsed = 50)
    {
        testMillis += elapsed;
        events = {one, tea, two};
        handler.handleState();
        actor.loop();
    }

    void pulses(int group, unsigned count)
    {
        while (count--)
        {
            if (group == 1)
                handler.groupOneFlowMeterPulseInterrupt();
            else
                handler.groupTwoFlowMeterPulseInterrupt();
        }
    }

    void selectProgramming()
    {
        tick(Event::CONTINUOUS);
        tick(Event::CONTINUOUS_HELD, Event::NONE, false, 2000);
    }
};

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
    VolumetricsHelper rebootVolumes{&rebootPreferences};
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

    machine.selectProgramming();
    machine.tick(Event::NONE, Event::LEFT_SINGLE_ESPRESSO);
    machine.pulses(2, 11);
    machine.tick();
    require(machine.state.groupTwoIsExtracting, "group two can own a learning extraction");
    machine.tick(Event::NONE, Event::CONTINUOUS);
    require(machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 11,
            "group two must save its own learned dose");

    machine.selectProgramming();
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
    GroupHeadButtonHandler buttons(&keys, &event, 1);
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
    require(testPins[RELAY_PUMP] == (config.relayConfig.pumpInverted ? LOW : HIGH),
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
    require(!machine.state.isFillingBoiler && !machine.state.boilerFillFault,
            "full boiler must stop normally without a latched fault");
}

void extractionSafetyLimits()
{
    for (uint32_t start : {1u, 0xfffff000u})
    {
        Machine machine;
        testMillis = start;
        machine.tick(Event::LEFT_SINGLE_ESPRESSO);
        machine.tick(Event::NONE, Event::NONE, false, 9999);
        require(machine.state.groupOneIsExtracting, "initial no-flow grace period must last ten seconds");
        machine.tick(Event::NONE, Event::NONE, false, 1);
#if BREWPILOT_HAS_FLOWMETERS
        require(!machine.state.groupOneIsExtracting, "no-flow must stop exactly at ten seconds, including clock rollover");
        machine.tick(Event::LEFT_SINGLE_ESPRESSO);
        machine.tick(Event::NONE, Event::NONE, false, 9000);
        machine.pulses(1, 3);
        machine.tick(Event::NONE, Event::NONE, false, 1);
        machine.tick(Event::NONE, Event::NONE, false, 9999);
        require(machine.state.groupOneIsExtracting, "new pulses must refresh the no-flow deadline");
        machine.tick(Event::NONE, Event::NONE, false, 1);
        require(!machine.state.groupOneIsExtracting, "stalled flow after initial pulses must also stop");
#else
        require(machine.state.groupOneIsExtracting, "machines without flowmeters must skip no-flow detection");
        machine.tick(Event::NONE, Event::NONE, false, 110000);
        require(!machine.state.groupOneIsExtracting, "no-flow opt-out must not disable maximum extraction duration");
#endif
        machine.tick(Event::CONTINUOUS);
        for (int second = 1; second <= 119; ++second)
        {
            machine.pulses(1, 1);
            machine.tick(Event::NONE, Event::NONE, false, 1000);
        }
        machine.tick(Event::NONE, Event::NONE, false, 999);
        require(machine.state.groupOneIsExtracting, "continuous extraction may run up to maximum duration");
        machine.tick(Event::NONE, Event::NONE, false, 1);
        require(!machine.state.groupOneIsExtracting, "continuous extraction must stop at 120 seconds despite flowing water");

        machine.selectProgramming();
        machine.tick(Event::LEFT_SINGLE_ESPRESSO);
        machine.pulses(1, 3);
        machine.tick();
        machine.tick(Event::NONE, Event::NONE, false, 120000);
        require(!machine.state.groupOneIsExtracting, "learning must also obey safety limits");
        require(machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 7,
                "safety abort must never save a partial learning extraction");
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

void boilerTimeoutRequiresAcknowledgement()
{
    for (uint32_t start : {1u, 0xfffff000u})
    {
        Machine machine;
        testMillis = start;
        testProbe = 3000;
        machine.tick(); // Energize probe.
        machine.tick(); // Start filling after the actual sample.
        require(machine.state.isFillingBoiler, "dry probe must start filling");
        machine.tick(Event::NONE, Event::NONE, false, 119000);
        machine.tick(Event::AUTO_BACKFLUSH, Event::CONTINUOUS, true, 999);
        require(machine.state.isFillingBoiler && machine.state.groupOneIsExtracting &&
                    machine.state.groupTwoIsExtracting && machine.state.isExtractingTeaWater,
                "water operations may run just before the fill deadline");
        machine.tick(Event::NONE, Event::NONE, false, 1);
        auto requireAllOff = [&] {
            require(!machine.state.isFillingBoiler && !machine.state.groupOneIsExtracting &&
                        !machine.state.groupTwoIsExtracting && !machine.state.isExtractingTeaWater,
                    "boiler timeout must stop every water operation, including backflush");
            require(testPins[RELAY_PUMP] == (config.relayConfig.pumpInverted ? HIGH : LOW),
                    "pump output must physically be commanded off");
            require(testPins[RELAY_BOILER_FILL] == (config.relayConfig.boilerFillInverted ? HIGH : LOW),
                    "fill valve must physically be commanded off");
        };
        requireAllOff();
        machine.tick(Event::CONTINUOUS_HELD); // Held before the fault: not an acknowledgement.
        testProbe = 0;
        machine.tick(Event::LEFT_SINGLE_ESPRESSO, Event::AUTO_BACKFLUSH, true, 5000);
        requireAllOff(); // Even a wet probe and new drink requests must not clear the latch.

        GroupButtonMatrixState keys{};
        Event event = Event::NONE;
        GroupHeadButtonHandler buttons(&keys, &event, 1);
        keys.continuous = true;
        for (int sample = 0; sample < 40; ++sample)
        {
            buttons.handle();
            machine.tick(event);
            requireAllOff(); // Acknowledgement itself must not start a drink.
        }
        machine.tick();
        machine.tick();
        machine.tick(Event::LEFT_SINGLE_ESPRESSO);
        require(machine.state.groupOneIsExtracting, "fresh press-and-hold must allow operation again");
        machine.tick(Event::CONTINUOUS);
        testProbe = 3000;
        for (int sample = 0; sample < 50; ++sample)
            machine.tick();
        require(machine.state.isFillingBoiler, "acknowledged machine can fill again");
        machine.tick(Event::NONE, Event::NONE, false, 120000);
        requireAllOff(); // Acknowledgement does not disable the next timeout.
    }
}

int main()
{
    try
    {
        failedStoragePreservesSettings();
        std::cout << "PASS: failed storage preserves settings; successful writes survive reboot\n";
        programmingDoesNotCaptureOtherDrinks();
        programmingCancelPreservesLastDose();
        std::cout << "PASS: programming ownership, tea learning and cancellation\n";
        stopWinsOverBackflush();
        std::cout << "PASS: STOP takes priority over backflush\n";
        normalDosesAndBoilerStillStop();
        std::cout << "PASS: normal doses, shared pump, tea duration and boiler filling\n";
        extractionSafetyLimits();
        backflushRemainsBoundedWithoutFlow();
        std::cout << "PASS: extraction timeouts, flow opt-out, clock rollover and bounded backflush\n";
        boilerTimeoutRequiresAcknowledgement();
        std::cout << "PASS: boiler timeout, all outputs off, explicit acknowledgement and repeat fault\n";
    }
    catch (const std::exception &error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
