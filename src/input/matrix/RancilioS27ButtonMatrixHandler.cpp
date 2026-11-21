// Compiled only for the machine with this panel.
#if defined(BREWPILOT_MACHINE_RANCILIO_S27)

#include "input/matrix/RancilioS27ButtonMatrixHandler.h"

#include <Arduino.h>

#include "config/machines/RancilioS27.h"

using namespace rancilio_s27::panel;

RancilioS27ButtonMatrixHandler::RancilioS27ButtonMatrixHandler(ButtonMatrixState &buttonMatrixState) : buttonMatrixState(buttonMatrixState) {}

void RancilioS27ButtonMatrixHandler::initializePins()
{
    pinMode(OUT_DOSE_BUTTONS, OUTPUT);
    pinMode(OUT_TEA_WATER, OUTPUT);

    // Dose inputs and tea use external 10k pull-ups (R22, R24-R27).
    // Dose 1
    pinMode(IN_DOSE_1, INPUT);
    // Dose 2
    pinMode(IN_DOSE_2, INPUT);
    // Dose 3
    pinMode(IN_DOSE_3, INPUT);
    // Dose 4
    pinMode(IN_DOSE_4, INPUT);
    // GPIO12: a permanent external pull-up risks the boot flash-voltage strap.
    pinMode(IN_CONTINUOUS, INPUT_PULLUP);

    pinMode(IN_TEA_WATER, INPUT);

    digitalWrite(OUT_DOSE_BUTTONS, LOW);
    digitalWrite(OUT_TEA_WATER, LOW);
}

void RancilioS27ButtonMatrixHandler::handle()
{
    buttonMatrixState.groupOne.leftSingleDose = !digitalRead(IN_DOSE_1);
    buttonMatrixState.groupOne.leftDoubleDose = !digitalRead(IN_DOSE_2);
    buttonMatrixState.groupOne.continuous = !digitalRead(IN_CONTINUOUS);
    buttonMatrixState.groupOne.rightSingleDose = !digitalRead(IN_DOSE_3);
    buttonMatrixState.groupOne.rightDoubleDose = !digitalRead(IN_DOSE_4);

    buttonMatrixState.tea = !digitalRead(IN_TEA_WATER);
}

#endif
