#include "RancilioS27ButtonMatrixHandler.h"

using Rancilio::IN_TEA_WATER;
using Rancilio::OUT_TEA_WATER;

RancilioS27ButtonMatrixHandler::RancilioS27ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState) : MachineButtonMatrixHandler(buttonMatrixState) {}

void RancilioS27ButtonMatrixHandler::initializePins()
{
    pinMode(OUT_DOSE_BUTTONS, OUTPUT);
    pinMode(OUT_TEA_WATER, OUTPUT);

    // Dose 1
    pinMode(IN_DOSE_1, INPUT_PULLUP);
    // Dose 2
    pinMode(IN_DOSE_2, INPUT_PULLUP);
    // Dose 3
    pinMode(IN_DOSE_3, INPUT_PULLUP);
    // Dose 4
    pinMode(IN_DOSE_4, INPUT_PULLUP);
    // Continuous
    pinMode(IN_CONTINUOUS, INPUT_PULLUP);

    pinMode(IN_TEA_WATER, INPUT_PULLUP);

    digitalWrite(OUT_DOSE_BUTTONS, LOW);
    digitalWrite(OUT_TEA_WATER, LOW);
}

void RancilioS27ButtonMatrixHandler::handle()
{
    buttonMatrixState->groupOne.leftSingleDose = !digitalRead(IN_DOSE_1);
    buttonMatrixState->groupOne.leftDoubleDose = !digitalRead(IN_DOSE_2);
    buttonMatrixState->groupOne.continuous = !digitalRead(IN_CONTINUOUS);
    buttonMatrixState->groupOne.rightSingleDose = !digitalRead(IN_DOSE_3);
    buttonMatrixState->groupOne.rightDoubleDose = !digitalRead(IN_DOSE_4);

    buttonMatrixState->tea = !digitalRead(IN_TEA_WATER);
}
