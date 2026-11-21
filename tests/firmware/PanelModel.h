#pragma once

#include "StateMachineFixture.h"

// Physical contacts from the two panel pinouts, independent of the firmware's
// IN_/OUT_ aliases. A closed contact pulls its input low only when its row is low.
struct PanelContact
{
    int input;
    int row;
};

inline constexpr Event panelEvents[] = {Event::LEFT_SINGLE_ESPRESSO, Event::LEFT_DOUBLE_ESPRESSO,
                                        Event::RIGHT_SINGLE_ESPRESSO, Event::RIGHT_DOUBLE_ESPRESSO,
                                        Event::CONTINUOUS};
inline constexpr unsigned stopKey = 4;
#if defined(BREWPILOT_MACHINE_RANCILIO_S27)
inline constexpr PanelContact panelContacts[][5] = {{{27, 33}, {26, 33}, {25, 33}, {36, 33}, {12, 33}}};
inline constexpr int outputOn = HIGH;
inline constexpr int outputOff = LOW;
#else
inline constexpr PanelContact panelContacts[][5] = {
    {{26, 33}, {27, 33}, {26, 32}, {27, 32}, {25, 33}},
    {{12, 13}, {36, 13}, {12, 14}, {36, 14}, {39, 13}}};
inline constexpr int outputOn = LOW;
inline constexpr int outputOff = HIGH;
#endif
inline constexpr unsigned panelGroups = sizeof(panelContacts) / sizeof(panelContacts[0]);
inline bool panelKeys[panelGroups][5]{};
inline bool panelTea = false;

// Model independently decoded contacts, not ghosting through the real keypad.
int readPanelPin(int pin);
void releasePanel();
