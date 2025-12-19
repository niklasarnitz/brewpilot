#ifndef BREWPILOT_RANCILIOS27BUTTONMATRIXHANDLER_H
#define BREWPILOT_RANCILIOS27BUTTONMATRIXHANDLER_H

#include "MachineButtonMatrixHandler.h"
#include "../../../StaticConfig.h"
#include "../../../utils/noop.h"
#include "Arduino.h"

#define OUT_DOSE_BUTTONS FLEXIBLE_IO_7
#define IN_DOSE_1 FLEXIBLE_IO_4
#define IN_DOSE_2 FLEXIBLE_IO_5
#define IN_DOSE_3 FLEXIBLE_IO_6
#define IN_DOSE_4 FLEXIBLE_IO_1
#define IN_CONTINUOUS FLEXIBLE_IO_2

class RancilioS27ButtonMatrixHandler : public MachineButtonMatrixHandler
{
public:
    RancilioS27ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState) : MachineButtonMatrixHandler(buttonMatrixState) {}

    void initializePins()
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

    void handle()
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
};

#endif // BREWPILOT_RANCILIOS27BUTTONMATRIXHANDLER_H
