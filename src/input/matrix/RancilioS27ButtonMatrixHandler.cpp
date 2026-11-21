#include "input/matrix/RancilioS27ButtonMatrixHandler.h"

#include <Arduino.h>

#include "config/BoardPins.h"

#define OUT_DOSE_BUTTONS FLEXIBLE_IO_7
#define IN_DOSE_1 FLEXIBLE_IO_4
#define IN_DOSE_2 FLEXIBLE_IO_5
#define IN_DOSE_3 FLEXIBLE_IO_6
#define IN_DOSE_4 FLEXIBLE_IO_1
#define IN_CONTINUOUS FLEXIBLE_IO_2

// Same connector pins as the M29 tea scan line, defined here for this panel.
#define OUT_TEA_WATER FLEXIBLE_IO_9
#define IN_TEA_WATER FLEXIBLE_IO_12

RancilioS27ButtonMatrixHandler::RancilioS27ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState) : MachineButtonMatrixHandler(buttonMatrixState) {}

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
    buttonMatrixState->groupOne.leftSingleDose = !digitalRead(IN_DOSE_1);
    buttonMatrixState->groupOne.leftDoubleDose = !digitalRead(IN_DOSE_2);
    buttonMatrixState->groupOne.continuous = !digitalRead(IN_CONTINUOUS);
    buttonMatrixState->groupOne.rightSingleDose = !digitalRead(IN_DOSE_3);
    buttonMatrixState->groupOne.rightDoubleDose = !digitalRead(IN_DOSE_4);

    buttonMatrixState->tea = !digitalRead(IN_TEA_WATER);

    // Logging ButtonMatrixState
    // Serial.println("ButtonMatrixState:");
    // Serial.printf("Group1: LSingle=%d, LDouble=%d, Cont=%d, RSingle=%d, RDouble=%d\n",
    //               buttonMatrixState->groupOne.leftSingleDose,
    //               buttonMatrixState->groupOne.leftDoubleDose,
    //               buttonMatrixState->groupOne.continuous,
    //               buttonMatrixState->groupOne.rightSingleDose,
    //               buttonMatrixState->groupOne.rightDoubleDose);
    // Serial.printf("Group2: LSingle=%d, LDouble=%d, Cont=%d, RSingle=%d, RDouble=%d\n",
    //               buttonMatrixState->groupTwo.leftSingleDose,
    //               buttonMatrixState->groupTwo.leftDoubleDose,
    //               buttonMatrixState->groupTwo.continuous,
    //               buttonMatrixState->groupTwo.rightSingleDose,
    //               buttonMatrixState->groupTwo.rightDoubleDose);
    // Serial.printf("Tea: %d\n", buttonMatrixState->tea);
}
