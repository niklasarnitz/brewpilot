#include "StateMachineFixture.h"
#include "hardware/handlers/InputHandler.h"

namespace
{
// Physical contacts from the two panel pinouts, independent of the firmware's
// IN_/OUT_ aliases. A closed contact pulls its input low only when its row is low.
struct PanelContact
{
    int input;
    int row;
};

constexpr Event panelEvents[] = {Event::LEFT_SINGLE_ESPRESSO, Event::LEFT_DOUBLE_ESPRESSO,
                                 Event::RIGHT_SINGLE_ESPRESSO, Event::RIGHT_DOUBLE_ESPRESSO,
                                 Event::CONTINUOUS};
constexpr unsigned stopKey = 4;
#if defined(BREWPILOT_MACHINE_RANCILIO_S27)
constexpr PanelContact panelContacts[][5] = {{{27, 33}, {26, 33}, {25, 33}, {36, 33}, {12, 33}}};
constexpr int outputOn = HIGH;
constexpr int outputOff = LOW;
#else
constexpr PanelContact panelContacts[][5] = {
    {{26, 33}, {27, 33}, {26, 32}, {27, 32}, {25, 33}},
    {{12, 13}, {36, 13}, {12, 14}, {36, 14}, {39, 13}}};
constexpr int outputOn = LOW;
constexpr int outputOff = HIGH;
#endif
constexpr unsigned panelGroups = sizeof(panelContacts) / sizeof(panelContacts[0]);
bool panelKeys[panelGroups][5]{};
bool panelTea = false;

int readPanelPin(int pin)
{
    for (unsigned group = 0; group < panelGroups; ++group)
        for (unsigned key = 0; key < 5; ++key)
        {
            const auto contact = panelContacts[group][key];
            if (contact.input == pin && panelKeys[group][key] && testPins[contact.row] == LOW)
                return LOW;
        }
    if (pin == 23 && panelTea && testPins[19] == LOW)
        return LOW;
    return HIGH;
}

void releasePanel()
{
    for (auto &group : panelKeys)
        for (bool &pressed : group)
            pressed = false;
    panelTea = false;
}

struct PanelDecoder
{
    ButtonEvent events{};
    InputHandler input{&events};
};

PanelDecoder &panelDecoder()
{
    // Match firmware's process-lifetime InputHandler, including its matrix
    // driver. Release keys between scenarios to reset the real edge counters.
    static PanelDecoder decoder;
    return decoder;
}

struct GpioMachine : StateMachineFixture
{
    GpioMachine()
    {
        releasePanel();
        testDigitalReadHook = readPanelPin;
        panelDecoder().input.readInputs();
        const unsigned settings[] = {5, 8, 11, 14};
        for (unsigned key = 0; key < 4; ++key)
            require(volumes.writeFlowMeterSetting(panelEvents[key], settings[key]), "seed physical panel dose");
        require(volumes.writeTeaWaterSetting(3000), "seed physical panel tea duration");
    }

    ~GpioMachine()
    {
        releasePanel();
        panelDecoder().input.readInputs();
        testDigitalReadHook = nullptr;
    }

    void tick(uint32_t elapsed = 50)
    {
        testMillis += elapsed;
        panelDecoder().input.readInputs();
        events = panelDecoder().events;
        handler.handleState();
        actor.loop();
    }

    void expectWater(bool one, bool two, bool tea, bool fill = false)
    {
        require(state.groupOneIsExtracting == one && state.groupTwoIsExtracting == two &&
                    state.isExtractingTeaWater == tea && state.isFillingBoiler == fill,
                "GPIO input scenario must produce the expected independent water states");
        require(testPins[RELAY_GROUP_ONE] == (one ? outputOn : outputOff) &&
                    testPins[RELAY_GROUP_TWO] == (two ? outputOn : outputOff) &&
                    testPins[RELAY_TEA] == (tea ? outputOn : outputOff) &&
                    testPins[RELAY_BOILER_FILL] == (fill ? outputOn : outputOff) &&
                    testPins[RELAY_PUMP] == ((one || two || fill) ? outputOn : outputOff),
                "physical valves and pump must match expected outputs and machine polarity");
    }

    void selectFromPanel()
    {
        panelKeys[0][stopKey] = true;
        tick();
        expectWater(true, false, false);
        for (int sample = 2; sample < 40; ++sample)
        {
            tick();
            expectWater(true, false, false);
        }
        tick();
        expectWater(false, false, false);
        require(events.groupOne == Event::CONTINUOUS_HELD && state.programmingTarget == ProgrammingTarget::SELECTING,
                "40 real GPIO samples must enter programming without water");
        tick();
        require(events.groupOne == Event::NONE && state.programmingTarget == ProgrammingTarget::SELECTING,
                "held panel key must not repeatedly enter programming");
        panelKeys[0][stopKey] = false;
        tick();
    }
};

void physicalDoseButtons()
{
    const unsigned targets[] = {5, 8, 11, 14};
    for (unsigned group = 0; group < panelGroups; ++group)
        for (unsigned key = 0; key < 4; ++key)
        {
            GpioMachine machine;
            machine.tick();
            machine.expectWater(false, false, false);
            panelKeys[group][key] = true;
            machine.tick();
            require(machine.events.groupOne == (group == 0 ? panelEvents[key] : Event::NONE) &&
                        machine.events.groupTwo == (group == 1 ? panelEvents[key] : Event::NONE),
                    "physical dose contact must decode to exactly its group and dose");
            machine.expectWater(group == 0, group == 1, false);
            machine.tick();
            require(machine.events.groupOne == Event::NONE && machine.events.groupTwo == Event::NONE,
                    "held physical dose contact must not repeat its event");
#if BREWPILOT_HAS_FLOWMETERS
            machine.pulses(group + 1, targets[key] - 1);
            machine.tick();
            machine.expectWater(group == 0, group == 1, false);
            machine.pulses(group + 1, 1);
            machine.tick();
#else
            machine.tick(10000);
            machine.expectWater(group == 0, group == 1, false);
            panelKeys[group][stopKey] = true;
            machine.tick();
#endif
            machine.expectWater(false, false, false);
            machine.tick();
            machine.expectWater(false, false, false);
            require(machine.volumes.getFlowMeterSetting(panelEvents[key]) == targets[key],
                    "physical normal extraction must not overwrite its setting");
        }
}

void physicalTeaAndStopPriority()
{
    GpioMachine machine;
    panelTea = true;
    machine.tick();
    require(machine.events.tea, "physical tea press must emit a tea event");
    machine.expectWater(false, false, true);
    for (int sample = 2; sample <= 40; ++sample)
    {
        machine.tick();
        require(!machine.events.tea, "holding tea, including its HELD sample, must not emit another press");
        machine.expectWater(false, false, true);
    }
    machine.tick(1049); // 39 x 50ms + 1049ms = 2999ms since the initial press.
    machine.expectWater(false, false, true);
    machine.tick(1);
    machine.expectWater(false, false, false);
    machine.tick();
    machine.expectWater(false, false, false);
    releasePanel();
    machine.tick();

    panelKeys[0][0] = panelKeys[0][3] = true;
    machine.tick();
    require(machine.events.groupOne == Event::AUTO_BACKFLUSH, "physical chord must start backflush");
    machine.expectWater(true, false, false);
    panelKeys[0][stopKey] = true;
    machine.tick();
    require(machine.events.groupOne == Event::CONTINUOUS, "physical stop must outrank the chord");
    machine.expectWater(false, false, false);
    panelKeys[0][0] = panelKeys[0][3] = false;
    machine.tick();
    panelKeys[0][0] = panelKeys[0][3] = true;
    machine.tick();
    machine.expectWater(false, false, false);
    require(machine.events.groupOne == Event::NONE, "held stop must block newly pressed chord keys");
    panelKeys[0][stopKey] = false;
    machine.tick();
    machine.expectWater(false, false, false);
    releasePanel();
    machine.tick();
    panelKeys[0][0] = panelKeys[0][3] = true;
    machine.tick();
    machine.expectWater(true, false, false);
    machine.tick(10000);
    machine.expectWater(false, false, false);
    panelKeys[0][stopKey] = true;
    machine.tick();
    machine.tick(5000);
    machine.expectWater(false, false, false);
}

void physicalFaultAcknowledgement()
{
    for (unsigned group = 0; group < panelGroups; ++group)
    {
        GpioMachine machine;
        testProbe = 1001;
        machine.tick();
        machine.tick();
        machine.expectWater(false, false, false, true);
        panelKeys[group][stopKey] = true;
        panelTea = true;
        machine.tick(119999);
        machine.expectWater(group == 0, group == 1, true, true);
        machine.tick(1);
        machine.expectWater(false, false, false);
        require(machine.state.boilerFillFault, "fill timeout must lock actual GPIO-started operations");
        for (int sample = 0; sample < 45; ++sample)
        {
            machine.tick();
            machine.expectWater(false, false, false);
            require(machine.state.boilerFillFault, "pre-fault held key must never acknowledge the lock");
        }
        releasePanel();
        machine.tick();
        panelKeys[group][stopKey] = true;
        for (int sample = 1; sample < 40; ++sample)
        {
            machine.tick();
            machine.expectWater(false, false, false);
            require(machine.state.boilerFillFault, "fresh press must not acknowledge before its 40th sample");
        }
        machine.tick();
        machine.expectWater(false, false, false);
        require(!machine.state.boilerFillFault && machine.state.programmingTarget == ProgrammingTarget::NONE,
                "40th fresh GPIO sample must acknowledge without starting water or programming");
        testProbe = 0;
        releasePanel();
        machine.tick();
        machine.tick();
        panelKeys[group][1] = true;
        machine.tick();
        machine.expectWater(group == 0, group == 1, false);
    }
}

void mixedPanelSessions()
{
    GpioMachine machine;
    // One machine and one real decoder throughout: later sessions must not
    // inherit held keys, programming ownership, tea timers or backflush phases.
    for (int session = 0; session < 24; ++session)
    {
        if (session % 3 == 0)
        {
            const unsigned key = (session / 3) % 4;
            panelKeys[0][key] = true;
            if (panelGroups == 2)
                panelKeys[1][(key + 1) % 4] = true;
            panelTea = true;
            machine.tick();
            machine.expectWater(true, panelGroups == 2, true);
            releasePanel();
            machine.tick();
            panelKeys[0][stopKey] = true;
            machine.tick();
            machine.expectWater(false, panelGroups == 2, true);
            if (panelGroups == 2)
                panelKeys[1][stopKey] = true;
            panelTea = true;
            machine.tick();
            machine.expectWater(false, false, false);
        }
        else if (session % 3 == 1)
        {
            panelKeys[0][0] = panelKeys[0][3] = true;
            if (panelGroups == 2)
                panelKeys[1][stopKey] = true;
            machine.tick();
            machine.expectWater(true, panelGroups == 2, false);
            releasePanel();
            machine.tick();
            panelKeys[0][stopKey] = true;
            if (panelGroups == 2)
                panelKeys[1][stopKey] = true;
            machine.tick();
            machine.expectWater(false, false, false);
        }
        else
        {
            machine.selectFromPanel();
            panelTea = true;
            machine.tick();
            machine.expectWater(false, false, true);
            require(machine.state.programmingTarget == ProgrammingTarget::TEA, "physical tea must own learning");
            panelTea = false;
            const uint32_t learned = 4100 + session * 7;
            machine.tick(learned - 50);
            machine.expectWater(false, false, true);
            panelTea = true;
            machine.tick();
            machine.expectWater(false, false, false);
            require(machine.volumes.getTeaWaterSetting() == learned &&
                        machine.state.programmingTarget == ProgrammingTarget::NONE,
                    "physical tea learning must save only this session's elapsed time and release ownership");
        }
        releasePanel();
        machine.tick();
        machine.expectWater(false, false, false);
        require(machine.state.programmingTarget == ProgrammingTarget::NONE, "mixed session must finish without stale programming");
        require(machine.volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 5 &&
                    machine.volumes.getFlowMeterSetting(Event::RIGHT_DOUBLE_ESPRESSO) == 14,
                "mixed drinks and tea learning must preserve group settings");
    }
}
} // namespace

void runInputOutputTests()
{
    physicalDoseButtons();
    physicalTeaAndStopPriority();
    physicalFaultAcknowledgement();
    mixedPanelSessions();
    std::cout << "PASS: physical GPIO panels, input-to-output doses/tea/STOP/fault acknowledgement and 24 mixed sessions\n";
}
