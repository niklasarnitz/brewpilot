#include "PanelModel.h"

int readPanelPin(int pin)
{
    if (pin == 12)
        require(testPinModes[pin] == INPUT_PULLUP, "GPIO12 needs its internal return pull-up");
    for (unsigned group = 0; group < panelGroups; ++group)
        for (unsigned key = 0; key < 5; ++key)
        {
            const auto contact = panelContacts[group][key];
            if (contact.input == pin && panelKeys[group][key] &&
                testPinModes[contact.row] == OUTPUT && testPins[contact.row] == LOW)
                return LOW;
        }
    if (pin == 23 && panelTea && testPinModes[19] == OUTPUT && testPins[19] == LOW)
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
