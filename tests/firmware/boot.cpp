#include "StateMachineFixture.h"
#include "../../src/main.cpp"

struct RestoreReadHook
{
    int (*previous)(int) = testDigitalReadHook;
    ~RestoreReadHook() { releasePanel(); testDigitalReadHook = previous; }
};

void testFirmwareBoot()
{
    RestoreReadHook restore;
    releasePanel();
    testDigitalReadHook = readPanelPin;
    testStorage.clear();
    testFailOpen = testFailWrite = false;
    testProbe = 0;
    testMillis = 1;
    for (auto &interrupt : testInterrupts)
        interrupt = {};
    testPinModes[16] = testPinModes[35] = -1;

    testStorage["FM_P_L_S"] = 3;
    setup();
    for (int pin : {2, 4, 5, 15, 18})
        require(testPinModes[pin] == OUTPUT && testPins[pin] == outputOff,
                "all valves and the pump must be inactive on boot");
    require(volumetricsHelper.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 3,
            "boot must restore the stored dose before the first loop");
    require(testPinModes[16] == INPUT && testPinModes[35] == INPUT,
            "setup must configure physical flow meter GPIO16 and GPIO35 as inputs");
    unsigned registrations = 0;
    for (const auto &interrupt : testInterrupts)
        registrations += interrupt.registrations;
#if BREWPILOT_HAS_FLOWMETERS
    require(registrations == 2 && testInterrupts[16].registrations == 1 && testInterrupts[35].registrations == 1 &&
                testInterrupts[16].callback && testInterrupts[35].callback &&
                testInterrupts[16].mode == FALLING && testInterrupts[35].mode == FALLING,
            "setup must attach exactly one falling-edge interrupt to each physical flow meter pin");
#else
    require(registrations == 0, "flowmeter-disabled setup must not register interrupts");
#endif

    const uint32_t before = testMillis;
    for (auto &group : panelKeys)
        group[0] = true;
    loop();
    require(testMillis == before + 50, "firmware loop must delay 50 milliseconds");
    require(state.groupOneIsExtracting && testPins[4] == outputOn && testPins[5] == outputOn,
            "first loop must drive the group one valve and pump");
#if defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
    require(state.groupTwoIsExtracting, "physical group two dose must start through firmware loop");
#endif
    releasePanel();
    loop();
#if BREWPILOT_HAS_FLOWMETERS
    for (int i = 0; i < 3; ++i)
        testInterrupts[16].callback();
    loop();
    require(!state.groupOneIsExtracting, "GPIO16 callbacks must finish group one's dose");
#if defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
    require(state.groupTwoIsExtracting, "GPIO16 callbacks must not advance group two");
#endif
    for (int i = 0; i < 3; ++i)
        testInterrupts[35].callback();
    loop();
#if defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
    require(!state.groupTwoIsExtracting, "GPIO35 callbacks must finish group two's dose");
#endif
#else
    loop();
    require(state.groupOneIsExtracting, "disabled flowmeters must not stop the physical dose automatically");
#endif
}
