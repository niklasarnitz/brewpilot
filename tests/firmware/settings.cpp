#include "StateMachineFixture.h"
#include "Scenarios.h"
#include "drivers/Relay.h"
#include "drivers/Solenoid.h"

void settingsKeyMapping()
{
    testStorage.clear();
    testFailOpen = testFailWrite = false;
    const Event doses[] = {Event::LEFT_SINGLE_ESPRESSO, Event::LEFT_DOUBLE_ESPRESSO,
                           Event::RIGHT_SINGLE_ESPRESSO, Event::RIGHT_DOUBLE_ESPRESSO};
    const char *keys[] = {"FM_P_L_S", "FM_P_L_D", "FM_P_R_S", "FM_P_R_D"};
    const uint32_t values[] = {101, 202, 303, 404};
    for (unsigned i = 0; i < 4; ++i)
        testStorage[keys[i]] = values[i];
    testStorage["T_M"] = 5050;

    PreferenceHelper preferences;
    VolumetricsHelper volumes{preferences};
    volumes.setup();
    for (unsigned i = 0; i < 4; ++i)
        require(volumes.getFlowMeterSetting(doses[i]) == values[i], "each stored dose key must load independently");
    require(volumes.getTeaWaterSetting() == 5050, "stored tea key must load independently");

    for (unsigned i = 0; i < 4; ++i)
    {
        const auto before = testStorage;
        require(volumes.writeFlowMeterSetting(doses[i], values[i] + 10), "each dose must be writable");
        auto expected = before;
        expected[keys[i]] = values[i] + 10;
        require(testStorage == expected, "dose write must modify only its own NVS key");
    }
    const auto beforeTea = testStorage;
    require(volumes.writeTeaWaterSetting(6060), "tea duration must be writable");
    auto expectedTea = beforeTea;
    expectedTea["T_M"] = 6060;
    require(testStorage == expectedTea, "tea write must modify only its own NVS key");
}

void settingsInvalidAndLoadFailure()
{
    testStorage.clear();
    testFailOpen = testFailWrite = false;
    PreferenceHelper preferences;
    VolumetricsHelper volumes{preferences};
    volumes.setup();
    require(volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 0 &&
                volumes.getTeaWaterSetting() == 0, "missing keys must load default zero values");
    require(preferences.getULong(PreferenceKey::INVALID, 77) == 77, "invalid key must return the supplied default");
    require(!preferences.setULong(PreferenceKey::INVALID, 123), "invalid preference key must not save");
    for (Event event : {Event::CONTINUOUS, Event::CONTINUOUS_HELD, Event::AUTO_BACKFLUSH, Event::NONE})
    {
        require(volumes.getFlowMeterSetting(event) == 0, "non-dose events must have no dose setting");
        require(!volumes.writeFlowMeterSetting(event, 123), "non-dose events must not save a dose");
    }
    require(testStorage.empty(), "invalid settings must not create NVS keys");

    require(volumes.writeFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO, 71), "seed active dose");
    require(volumes.writeTeaWaterSetting(8000), "seed active tea");
    testStorage["FM_P_L_S"] = 99;
    testStorage["T_M"] = 9000;
    testFailOpen = true;
    volumes.setup();
    require(volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 71 &&
                volumes.getTeaWaterSetting() == 8000, "failed NVS load must preserve active settings");
    PreferenceHelper freshPreferences;
    VolumetricsHelper fresh{freshPreferences};
    fresh.setup();
    require(fresh.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 0 &&
                fresh.getTeaWaterSetting() == 0, "failed initial load must leave default settings");
    testFailOpen = false;
    volumes.setup();
    require(volumes.getFlowMeterSetting(Event::LEFT_SINGLE_ESPRESSO) == 99 &&
                volumes.getTeaWaterSetting() == 9000, "load must recover after NVS becomes available");
}

void standaloneDeviceDrivers()
{
    for (int inverted = 0; inverted < 2; ++inverted)
    {
        const int relayPin = 50 + inverted;
        const int solenoidPin = 52 + inverted;
        const int on = inverted ? LOW : HIGH;
        const int off = inverted ? HIGH : LOW;
        Relay relay(relayPin, "test relay", inverted);
        Solenoid solenoid(solenoidPin, "test solenoid", inverted);
        require(testPinModes[relayPin] == OUTPUT && testPinModes[solenoidPin] == OUTPUT &&
                    !relay.isEnabled() && !solenoid.isOpen() &&
                    testPins[relayPin] == off && testPins[solenoidPin] == off,
                "drivers must initialize their independent outputs inactive");
        for (int repeat = 0; repeat < 2; ++repeat)
        {
            relay.setEnabled(true);
            solenoid.setOpen(true);
            require(relay.isEnabled() && solenoid.isOpen() &&
                        testPins[relayPin] == on && testPins[solenoidPin] == on,
                    "enabled drivers must report active and drive their configured polarity");
        }
        for (int repeat = 0; repeat < 2; ++repeat)
        {
            relay.setEnabled(false);
            solenoid.setOpen(false);
            require(!relay.isEnabled() && !solenoid.isOpen() &&
                        testPins[relayPin] == off && testPins[solenoidPin] == off,
                    "disabled drivers must report inactive and drive their configured polarity");
        }
    }
}
