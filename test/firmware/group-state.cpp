#include "StateMachineFixture.h"

namespace
{
constexpr Event doses[] = {Event::LEFT_SINGLE_ESPRESSO, Event::LEFT_DOUBLE_ESPRESSO,
                           Event::RIGHT_SINGLE_ESPRESSO, Event::RIGHT_DOUBLE_ESPRESSO};

// Reuse the real persistence and state fixture, but invoke only the public
// group handler API so boiler/tea scheduling cannot hide a group transition.
struct GroupFixture : StateMachineFixture
{
    bool extracting = false;
    Event event = Event::NONE;
    GroupHeadStateHandler group;
    ProgrammingTarget own;

    explicit GroupFixture(int number = 1)
        : group(&extracting, &event, &volumes, &state.programmingTarget, number),
          own(number == 1 ? ProgrammingTarget::GROUP_ONE : ProgrammingTarget::GROUP_TWO) {}

    void step(Event next = Event::NONE, uint32_t elapsed = 0)
    {
        testMillis += elapsed;
        event = next;
        group.handleState();
    }

    void flow(unsigned count)
    {
        while (count--)
            group.flowMeterPulseInterrupt();
    }

    void select()
    {
        step(Event::CONTINUOUS);
        require(extracting, "continuous starts extraction before selection");
        step(Event::CONTINUOUS_HELD, 2000);
        require(!extracting && state.programmingTarget == ProgrammingTarget::SELECTING,
                "held continuous stops water and opens programming selection");
    }
};

void doseContracts()
{
    for (int number : {1, 2})
        for (Event dose : doses)
        {
            GroupFixture f(number);
            f.step();
            f.step(Event::CONTINUOUS_HELD);
            f.step(dose);
            require(!f.extracting, "unset doses and idle held events must not start water");
            require(f.volumes.writeFlowMeterSetting(dose, 7), "seed dose");
            f.flow(20);
            f.step(dose);
            require(f.extracting, "configured dose starts");
            f.step(Event::CONTINUOUS_HELD);
            require(f.extracting && f.state.programmingTarget == ProgrammingTarget::NONE,
                    "held during a volumetric dose does not select programming");
            f.flow(6);
            f.step();
            require(f.extracting, "dose remains active one pulse below its target");
            f.flow(1);
            f.step();
            require(!f.extracting, "dose stops at its exact target, excluding idle pulses");
            f.flow(30);
            f.step(dose);
            f.flow(9);
            f.step(dose);
            require(!f.extracting, "overshoot stops without restarting on a dose event");
            f.step(dose);
            f.flow(2);
            f.step(Event::CONTINUOUS);
            require(!f.extracting && f.volumes.getFlowMeterSetting(dose) == 7,
                    "manual cancellation must retain the dose");
        }
}

void continuousAndOwnership()
{
    GroupFixture f;
    f.step(Event::CONTINUOUS);
    f.flow(100);
    f.step(Event::LEFT_SINGLE_ESPRESSO);
    require(f.extracting, "dose events do not replace an active continuous extraction");
    f.step(Event::CONTINUOUS);
    require(!f.extracting, "second continuous event stops");
    f.select();
    f.step(Event::CONTINUOUS_HELD);
    require(!f.extracting, "held alone cannot choose a dose");
    f.step(Event::AUTO_BACKFLUSH);
    require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::SELECTING,
            "backflush is blocked during selection");
    f.step(Event::CONTINUOUS);
    require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::NONE,
            "continuous cancels idle selection without starting water");

    for (ProgrammingTarget other : {ProgrammingTarget::GROUP_TWO, ProgrammingTarget::TEA})
    {
        f.state.programmingTarget = other;
        f.step(Event::AUTO_BACKFLUSH);
        require(!f.extracting, "backflush cannot take another programming owner");
        f.step(Event::CONTINUOUS);
        f.step(Event::CONTINUOUS_HELD);
        require(f.extracting && f.state.programmingTarget == other,
                "held continuous cannot take another programming owner");
        f.group.stop();
        require(!f.extracting && f.state.programmingTarget == other,
                "public stop preserves another owner's programming");
        require(f.volumes.writeFlowMeterSetting(doses[0], 3), "seed other-owner dose");
        f.step(doses[0]);
        f.flow(3);
        f.step();
        require(!f.extracting && f.state.programmingTarget == other,
                "normal dose finishes independently of another programming owner");
    }
    f.state.programmingTarget = ProgrammingTarget::NONE;
    f.step(Event::CONTINUOUS);
    f.state.programmingTarget = ProgrammingTarget::SELECTING;
    f.step(Event::CONTINUOUS);
    require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::SELECTING,
            "stopping existing water must not cancel another group's selection");
}

void programmingContracts()
{
    for (int number : {1, 2})
        for (Event dose : doses)
        {
            GroupFixture f(number);
            f.select();
            f.flow(20);
            f.step(dose);
            require(f.extracting && f.state.programmingTarget == f.own,
                    "unset dose can acquire programming ownership");
            f.flow(11);
            f.step(Event::CONTINUOUS_HELD);
            require(f.extracting && f.state.programmingTarget == f.own,
                    "learning ignores old target and held continuous");
            f.step(Event::CONTINUOUS);
            require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::NONE,
                    "save ends learning and releases ownership");
            require(f.volumes.getFlowMeterSetting(dose) == 11,
                    "learning saves only pulses since the selected dose began");
            PreferenceHelper rebootPreferences;
            VolumetricsHelper reboot{&rebootPreferences};
            reboot.setup();
            require(reboot.getFlowMeterSetting(dose) == 11, "learned dose survives reload");
            for (Event untouched : doses)
                if (untouched != dose)
                    require(reboot.getFlowMeterSetting(untouched) == 0, "save changes only selected dose");

            f.select();
            f.step(dose);
            f.flow(14);
            f.step();
            require(f.extracting, "learning outlasts the previously saved target");
            f.group.stop();
            require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::NONE &&
                        f.volumes.getFlowMeterSetting(dose) == 11,
                    "public stop aborts learning without saving");

            for (int failure : {0, 1, 2})
            {
                f.select();
                f.step(dose);
                if (failure != 0)
                    f.flow(19);
                testFailOpen = failure == 1;
                testFailWrite = failure == 2;
                f.step(Event::CONTINUOUS);
                require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::NONE &&
                            f.volumes.getFlowMeterSetting(dose) == 11,
                        "zero pulses and storage failures exit learning but preserve old dose");
                testFailOpen = testFailWrite = false;
            }
        }
}

void backflushContracts()
{
    GroupFixture f;
    f.step(Event::AUTO_BACKFLUSH);
    for (int cycle = 1; cycle <= 5; ++cycle)
    {
        require(f.extracting, "backflush starts every extraction phase");
        f.step(Event::CONTINUOUS_HELD, 9999);
        require(f.extracting && f.state.programmingTarget == ProgrammingTarget::NONE,
                "backflush has priority over held programming and no-flow safety");
        f.step(Event::LEFT_SINGLE_ESPRESSO, 1);
        require(!f.extracting, "backflush extraction ends at ten seconds");
        if (cycle < 5)
        {
            f.step(Event::AUTO_BACKFLUSH, 4999);
            require(!f.extracting, "backflush pause remains off before five seconds");
            f.step(Event::NONE, 1);
            require(f.extracting, "backflush pause ends at five seconds");
        }
    }
    f.step(Event::NONE, 5000);
    require(!f.extracting, "fifth backflush extraction ends the sequence");
    f.step(Event::CONTINUOUS);
    require(f.extracting, "ordinary operation resumes after completed backflush");
    f.group.stop();

    for (bool pause : {false, true})
        for (bool publicStop : {false, true})
        {
            f.step(Event::AUTO_BACKFLUSH);
            if (pause)
                f.step(Event::NONE, 10000);
            if (publicStop)
                f.group.stop();
            else
                f.step(Event::CONTINUOUS);
            f.step(Event::NONE, 20000);
            require(!f.extracting, "stop cancels both phases permanently");
        }

    f.step(Event::CONTINUOUS);
    f.step(Event::AUTO_BACKFLUSH);
    f.step(Event::NONE, 10000);
    require(!f.extracting, "backflush can replace a normal extraction");
    f.state.programmingTarget = ProgrammingTarget::SELECTING;
    f.step(Event::CONTINUOUS);
    f.step(Event::NONE, 5000);
    require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::SELECTING,
            "backflush cancellation preserves another group's programming selection");
    f.step(Event::CONTINUOUS);
    require(f.state.programmingTarget == ProgrammingTarget::NONE && !f.extracting,
            "a subsequent idle continuous event cancels selection");
}

void safetyContracts()
{
    for (uint32_t start : {1u, 0xfffff000u})
    {
        GroupFixture f;
        testMillis = start;
        f.step(Event::CONTINUOUS);
        f.step(Event::NONE, 9999);
        require(f.extracting, "no-flow grace lasts until the boundary");
        f.step(Event::NONE, 1);
#if BREWPILOT_HAS_FLOWMETERS
        require(!f.extracting, "no-flow stops at ten seconds across clock rollover");
        f.step(Event::CONTINUOUS);
        f.flow(1);
        f.step(Event::NONE, 9000);
        f.step(Event::NONE, 9999);
        require(f.extracting, "observed flow refreshes the deadline");
        f.step(Event::NONE, 1);
        require(!f.extracting, "flow stall stops at the refreshed deadline");
#else
        require(f.extracting, "flowmeter opt-out leaves extraction active without pulses");
        f.step(Event::NONE, 110000);
        require(!f.extracting, "flowmeter opt-out retains maximum-duration safety");
#endif
        for (bool learning : {false, true})
        {
            require(f.volumes.writeFlowMeterSetting(doses[0], 7), "seed safety dose");
            if (learning)
            {
                f.select();
                f.step(doses[0]);
            }
            else
                f.step(Event::CONTINUOUS);
            for (int second = 1; second <= 119; ++second)
            {
                f.flow(1);
                f.step(Event::NONE, 1000);
            }
            f.step(Event::NONE, 999);
            require(f.extracting, "flowing extraction survives until 119999 milliseconds");
            f.flow(1);
            f.step(Event::CONTINUOUS, 1);
            require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::NONE,
                    "maximum duration wins over stop/save event at exactly 120 seconds");
            require(f.volumes.getFlowMeterSetting(doses[0]) == 7,
                    "safety timeout never saves a partial learning dose");
        }
#if BREWPILOT_HAS_FLOWMETERS
        f.select();
        f.step(doses[0]);
        f.flow(2);
        f.step();
        f.step(Event::CONTINUOUS, 10000);
        require(!f.extracting && f.state.programmingTarget == ProgrammingTarget::NONE &&
                    f.volumes.getFlowMeterSetting(doses[0]) == 7,
                "no-flow timeout wins over learning save");
#endif
    }
}
} // namespace

int runGroupStateTests()
{
    try
    {
        doseContracts();
        continuousAndOwnership();
        programmingContracts();
        backflushContracts();
        safetyContracts();
        std::cout << "PASS: group doses, continuous, programming, backflush and safety (flowmeters="
                  << BREWPILOT_HAS_FLOWMETERS << ")\n";
    }
    catch (const std::exception &error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
