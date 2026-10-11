#include "StateMachineFixture.h"

namespace
{
struct BoilerTest : StateMachineFixture
{
    BoilerStateHandler boiler{&state.isFillingBoiler, &state.boilerFillFault};

    void step(uint32_t elapsed = 0)
    {
        testMillis += elapsed;
        boiler.handleState();
        actor.loop();
        require(testPins[RELAY_BOILER_FILL] ==
                    ((state.isFillingBoiler != config.relayConfig.boilerFillInverted) ? HIGH : LOW),
                "fill valve must reflect filling state with configured polarity");
        require(testPins[RELAY_PUMP] ==
                    ((state.isFillingBoiler != config.relayConfig.pumpInverted) ? HIGH : LOW),
                "pump must reflect filling state with configured polarity");
    }

    void sample(uint16_t probe, uint32_t elapsed = 2001)
    {
        testProbe = probe;
        step(elapsed);
        require(testPins[OUT_BOILER_VOLTAGE] == HIGH,
                "probe voltage must be enabled before sampling");
        step();
        require(testPins[OUT_BOILER_VOLTAGE] == LOW,
                "probe voltage must be disabled after sampling");
    }
};

void boilerSamplingAndOverfill()
{
    BoilerTest machine;
    require(testPins[OUT_BOILER_VOLTAGE] == LOW, "constructor must disable probe voltage");
    testProbe = 1001;
    machine.step();
    require(testPins[OUT_BOILER_VOLTAGE] == HIGH && !machine.state.isFillingBoiler,
            "initial dry probe must settle without starting filling");
    testProbe = 1000;
    machine.step();
    require(!machine.state.isFillingBoiler, "1000 is full; sample must use settled reading");
    testProbe = 1001;
    machine.step(2000);
    require(testPins[OUT_BOILER_VOLTAGE] == LOW && !machine.state.isFillingBoiler,
            "exactly 2000ms must not prepare a new sample");
    machine.step(1);
    require(testPins[OUT_BOILER_VOLTAGE] == HIGH && !machine.state.isFillingBoiler,
            "2001ms must prepare but not sample the dry probe");
    machine.step();
    require(machine.state.isFillingBoiler, "1001 is dry and must start filling");
    machine.sample(1001);
    require(machine.state.isFillingBoiler, "still-dry boiler must keep filling");
    machine.sample(1000);
    require(machine.state.isFillingBoiler, "first full sample must enter intentional overfill");
    machine.step(2000);
    require(machine.state.isFillingBoiler && testPins[OUT_BOILER_VOLTAGE] == LOW,
            "overfill must still run at exactly 2000ms");
    machine.sample(1001, 1);
    require(!machine.state.isFillingBoiler && !machine.state.boilerFillFault,
            "overfill must stop even when its sample is dry");
    machine.sample(1001);
    require(machine.state.isFillingBoiler, "dry sample after overfill must restart filling");
}

void boilerTimeoutAndAcknowledgement()
{
    for (uint32_t start : {1u, 0xfffff000u})
    {
        for (bool overfill : {false, true})
        {
            BoilerTest machine;
            testMillis = start;
            machine.sample(1001, 0);
            if (overfill)
            {
                machine.sample(1000);
                machine.step(117998);
            }
            else
                machine.step(119999);
            require(machine.state.isFillingBoiler && !machine.state.boilerFillFault,
                    "fill deadline must not fire at 119999ms, even across rollover");
            require(testPins[OUT_BOILER_VOLTAGE] == HIGH,
                    "timeout scenario must include an energized settling probe");
            machine.step(1);
            require(!machine.state.isFillingBoiler && machine.state.boilerFillFault &&
                        testPins[OUT_BOILER_VOLTAGE] == LOW,
                    "120000ms must latch fault and turn off filling and probe, including overfill");
            testProbe = 1000;
            machine.step(5000);
            require(machine.state.boilerFillFault && !machine.state.isFillingBoiler &&
                        testPins[OUT_BOILER_VOLTAGE] == LOW,
                    "wet probe must not clear a latched fault");
            machine.boiler.acknowledgeFault();
            require(!machine.state.boilerFillFault && !machine.state.isFillingBoiler,
                    "explicit acknowledgement must clear fault without filling");
            machine.sample(1000, 0);
            require(!machine.state.isFillingBoiler, "acknowledgement must reset overfill state");
            machine.sample(1001);
            require(machine.state.isFillingBoiler, "acknowledged handler must permit fresh filling");
            machine.step(120000);
            require(machine.state.boilerFillFault && !machine.state.isFillingBoiler,
                    "fresh filling must have a fresh active timeout");
        }
    }
}

struct TeaTest : StateMachineFixture
{
    TeaStateHandler tea{&events.tea, &state.isExtractingTeaWater,
                        &volumes, &state.programmingTarget};

    void step(bool pressed = false, uint32_t elapsed = 0)
    {
        testMillis += elapsed;
        events.tea = pressed;
        tea.handleState();
        actor.loop();
        require(testPins[RELAY_TEA] ==
                    ((state.isExtractingTeaWater != config.relayConfig.teaInverted) ? HIGH : LOW),
                "tea valve must reflect extraction state with configured polarity");
    }
};

void teaNormalOperation()
{
    TeaTest machine;
    machine.step();
    machine.step(true);
    require(!machine.state.isExtractingTeaWater, "missing setting must prevent normal tea start");
    require(machine.volumes.writeTeaWaterSetting(3000), "normal tea setting must save");
    machine.step();
    require(!machine.state.isExtractingTeaWater, "configured tea must still require a press");
    machine.step(true);
    require(machine.state.isExtractingTeaWater, "press must start configured tea");
    machine.step(false, 2999);
    require(machine.state.isExtractingTeaWater, "tea must remain active one millisecond before limit");
    machine.step(false, 1);
    require(!machine.state.isExtractingTeaWater, "tea must stop exactly at configured limit");
    machine.step(true);
    machine.step(true, 100);
    require(!machine.state.isExtractingTeaWater && machine.volumes.getTeaWaterSetting() == 3000,
            "manual stop must not overwrite normal tea duration");
    machine.state.programmingTarget = ProgrammingTarget::SELECTING;
    machine.step();
    require(machine.state.programmingTarget == ProgrammingTarget::SELECTING &&
                !machine.state.isExtractingTeaWater,
            "selection without press must not start tea");
}

void teaNormalRollover()
{
    TeaTest machine;
    require(machine.volumes.writeTeaWaterSetting(3000), "seed rollover tea duration");
    testMillis = 0xfffffa24u; // Wrap 1500ms after start, well before the deadline.
    machine.step(true);
    for (uint32_t elapsed : {1499u, 1500u, 1501u, 2999u, 3000u})
    {
        testMillis = uint32_t(0xfffffa24u + elapsed);
        machine.step();
        const bool expected = elapsed < 3000;
        if (machine.state.isExtractingTeaWater != expected)
            throw std::runtime_error("tea rollover elapsed=" + std::to_string(elapsed) +
                                     " expected active=" + std::to_string(expected) +
                                     " actual=" + std::to_string(machine.state.isExtractingTeaWater));
    }
}

void teaLearningRollover()
{
    for (uint32_t duration : {2999u, 3000u, 4100u})
    {
        TeaTest machine;
        require(machine.volumes.writeTeaWaterSetting(3000), "seed prior learning duration");
        machine.state.programmingTarget = ProgrammingTarget::SELECTING;
        testMillis = 0xfffffa24u;
        machine.step(true);
        for (uint32_t elapsed : {1499u, 1500u, 1501u})
        {
            testMillis = uint32_t(0xfffffa24u + elapsed);
            machine.step();
            require(machine.state.isExtractingTeaWater &&
                        machine.state.programmingTarget == ProgrammingTarget::TEA,
                    "tea learning stays active before, exactly at and after wrap");
        }
        testMillis = uint32_t(0xfffffa24u + duration);
        machine.step();
        require(machine.state.isExtractingTeaWater, "learning ignores normal duration boundary");
        machine.step(true);
        require(!machine.state.isExtractingTeaWater &&
                    machine.state.programmingTarget == ProgrammingTarget::NONE,
                "rollover learning save stops water and releases ownership");
        PreferenceHelper rebootPreferences;
        VolumetricsHelper rebootVolumes{&rebootPreferences};
        rebootVolumes.setup();
        // Independent duration, not subtraction using the firmware's host-width type.
        const auto actual = machine.volumes.getTeaWaterSetting();
        const auto reloaded = rebootVolumes.getTeaWaterSetting();
        if (actual != duration || reloaded != duration)
            throw std::runtime_error("tea learning rollover expected=" + std::to_string(duration) +
                                     " actual=" + std::to_string(actual) +
                                     " reload=" + std::to_string(reloaded));
    }
}

void teaProgramming()
{
    // Successful save, both persistence failure modes, and zero-duration rejection.
    for (int failure = 0; failure < 4; ++failure)
    {
        TeaTest machine;
        if (failure != 0)
            require(machine.volumes.writeTeaWaterSetting(3000), "prior setting must save");
        machine.state.programmingTarget = ProgrammingTarget::SELECTING;
        machine.step(true);
        require(machine.state.isExtractingTeaWater && machine.state.programmingTarget == ProgrammingTarget::TEA,
                "selection press must start learning even with missing setting");
        if (failure != 3)
        {
            machine.step(false, 4100);
            require(machine.state.isExtractingTeaWater,
                    "learning tea must ignore missing or expired normal duration");
        }
        testFailOpen = failure == 1;
        testFailWrite = failure == 2;
        machine.step(true);
        require(!machine.state.isExtractingTeaWater && machine.state.programmingTarget == ProgrammingTarget::NONE,
                "learning stop must exit programming regardless of save result");
        const unsigned long expected = failure == 0 ? 4100 : 3000;
        require(machine.volumes.getTeaWaterSetting() == expected,
                "successful learning must save elapsed time; failures must preserve old duration");
        testFailOpen = testFailWrite = false;
        PreferenceHelper rebootPreferences;
        VolumetricsHelper rebootVolumes{&rebootPreferences};
        rebootVolumes.setup();
        require(rebootVolumes.getTeaWaterSetting() == expected,
                "persisted duration must match independent expected value after reboot");
    }
}
} // namespace

int runBoilerTeaStateTests()
{
    try
    {
        boilerSamplingAndOverfill();
        boilerTimeoutAndAcknowledgement();
        teaNormalOperation();
        teaProgramming();
        teaNormalRollover();
        teaLearningRollover();
        std::cout << "PASS: boiler sampling, thresholds, overfill, timeout/ack and tea normal/programming states\n";
    }
    catch (const std::exception &error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
