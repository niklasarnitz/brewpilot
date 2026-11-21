#include "PanelModel.h"
#include "Scenarios.h"
#include "StateMachineFixture.h"

// Firmware entry points and globals from src/main.cpp.
extern State state;
extern VolumetricsHelper volumetricsHelper;
void setup();
void loop();

namespace
{
struct RestoreReadHook
{
    int (*previous)(int) = testDigitalReadHook;
    ~RestoreReadHook() { releasePanel(); testDigitalReadHook = previous; }
};
} // namespace

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
                "all valves and the pump must be inactive on boot, including an absent group's");
    require(volumetricsHelper.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 3,
            "boot must restore the stored dose before the first loop");
    require(testPinModes[16] == INPUT && (testPinModes[35] == INPUT) == machineConfig.groupTwoActive,
            "setup must configure the flow meter input of each active group only");
    unsigned registrations = 0;
    for (const auto &interrupt : testInterrupts)
        registrations += interrupt.registrations;
    if constexpr (machineConfig.hasFlowMeters)
    {
        const unsigned groupTwo = machineConfig.groupTwoActive;
        require(registrations == 1 + groupTwo && testInterrupts[16].registrations == 1 &&
                    testInterrupts[35].registrations == groupTwo && testInterrupts[16].callback &&
                    bool(testInterrupts[35].callback) == bool(groupTwo) && testInterrupts[16].mode == FALLING &&
                    (testInterrupts[35].mode == FALLING) == bool(groupTwo),
                "setup must attach one falling-edge interrupt per active group's flow meter, and none otherwise");
    }
    else
    {
        require(registrations == 0, "flowmeter-disabled setup must not register interrupts");
    }

    const uint32_t before = testMillis;
    for (auto &group : panelKeys)
        group[0] = true;
    loop();
    require(testMillis == before + 50, "firmware loop must delay 50 milliseconds");
    require(state.groupOneIsExtracting && testPins[4] == outputOn && testPins[5] == outputOn,
            "first loop must drive the group one valve and pump");
    require(state.groupTwoIsExtracting == machineConfig.groupTwoActive,
            "a physical group two dose must start through the firmware loop exactly when group two exists");
    releasePanel();
    loop();
    if constexpr (machineConfig.hasFlowMeters)
    {
        for (int i = 0; i < 3; ++i)
            testInterrupts[16].callback();
        loop();
        require(!state.groupOneIsExtracting, "GPIO16 callbacks must finish group one's dose");
        if (machineConfig.groupTwoActive)
        {
            require(state.groupTwoIsExtracting, "GPIO16 callbacks must not advance group two");
            for (int i = 0; i < 3; ++i)
                testInterrupts[35].callback();
            loop();
            require(!state.groupTwoIsExtracting, "GPIO35 callbacks must finish group two's dose");
        }
    }
    else
    {
        loop();
        require(state.groupOneIsExtracting, "disabled flowmeters must not stop the physical dose automatically");
    }
}
