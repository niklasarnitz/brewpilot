#ifndef BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H
#define BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H

#define IN_GROUP_TWO_BIG_DRINKS FLEXIBLE_IO_1
#define IN_GROUP_TWO_SMALL_DRINKS FLEXIBLE_IO_2
#define IN_GROUP_TWO_STOP FLEXIBLE_IO_3
#define IN_GROUP_ONE_BIG_DRINKS FLEXIBLE_IO_4
#define IN_GROUP_ONE_SMALL_DRINKS FLEXIBLE_IO_5
#define IN_GROUP_ONE_STOP FLEXIBLE_IO_6

#define OUT_GROUP_ONE_LEFT_HALF_AND_STOP FLEXIBLE_IO_7
#define OUT_GROUP_ONE_RIGHT_HALF FLEXIBLE_IO_8

#define OUT_TEA_WATER FLEXIBLE_IO_9

#define OUT_GROUP_TWO_LEFT_HALF_AND_STOP FLEXIBLE_IO_10
#define OUT_GROUP_TWO_RIGHT_HALF FLEXIBLE_IO_11

#define IN_TEA_WATER FLEXIBLE_IO_12

#include "MachineButtonMatrixHandler.h"
#include "../../../StaticConfig.h"
#include "../../../utils/noop.h"
#include "Arduino.h"

class LaCimbaliM29ButtonMatrixHandler : public MachineButtonMatrixHandler
{
    static void selectScanLine(int pin)
    {
        // Preload LOW before enabling the output so it cannot pulse HIGH.
        digitalWrite(pin, LOW);
        pinMode(pin, OUTPUT);
        noop();
    }

public:
    LaCimbaliM29ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState) : MachineButtonMatrixHandler(buttonMatrixState) {}

    void initializePins()
    {
        pinMode(OUT_GROUP_ONE_LEFT_HALF_AND_STOP, INPUT);
        pinMode(OUT_GROUP_ONE_RIGHT_HALF, INPUT);
        pinMode(OUT_GROUP_TWO_LEFT_HALF_AND_STOP, INPUT);
        pinMode(OUT_GROUP_TWO_RIGHT_HALF, INPUT);
        pinMode(OUT_TEA_WATER, INPUT);

        // R22-R27 provide external 10k pull-ups on these six inputs.
        pinMode(IN_GROUP_ONE_BIG_DRINKS, INPUT);
        pinMode(IN_GROUP_ONE_SMALL_DRINKS, INPUT);
        pinMode(IN_GROUP_ONE_STOP, INPUT);

        pinMode(IN_GROUP_TWO_BIG_DRINKS, INPUT);
        // GPIO12: a permanent external pull-up risks the boot flash-voltage strap.
        pinMode(IN_GROUP_TWO_SMALL_DRINKS, INPUT_PULLUP);
        pinMode(IN_GROUP_TWO_STOP, INPUT);

        pinMode(IN_TEA_WATER, INPUT);
    }

    void handle()
    {
        selectScanLine(OUT_GROUP_ONE_LEFT_HALF_AND_STOP);

        buttonMatrixState->groupOne.leftSingleDose = !digitalRead(IN_GROUP_ONE_SMALL_DRINKS);
        buttonMatrixState->groupOne.leftDoubleDose = !digitalRead(IN_GROUP_ONE_BIG_DRINKS);
        buttonMatrixState->groupOne.continuous = !digitalRead(IN_GROUP_ONE_STOP);

        pinMode(OUT_GROUP_ONE_LEFT_HALF_AND_STOP, INPUT);
        selectScanLine(OUT_GROUP_ONE_RIGHT_HALF);

        buttonMatrixState->groupOne.rightSingleDose = !digitalRead(IN_GROUP_ONE_SMALL_DRINKS);
        buttonMatrixState->groupOne.rightDoubleDose = !digitalRead(IN_GROUP_ONE_BIG_DRINKS);

        pinMode(OUT_GROUP_ONE_RIGHT_HALF, INPUT);
        selectScanLine(OUT_GROUP_TWO_LEFT_HALF_AND_STOP);

        buttonMatrixState->groupTwo.leftSingleDose = !digitalRead(IN_GROUP_TWO_SMALL_DRINKS);
        buttonMatrixState->groupTwo.leftDoubleDose = !digitalRead(IN_GROUP_TWO_BIG_DRINKS);
        buttonMatrixState->groupTwo.continuous = !digitalRead(IN_GROUP_TWO_STOP);

        pinMode(OUT_GROUP_TWO_LEFT_HALF_AND_STOP, INPUT);
        selectScanLine(OUT_GROUP_TWO_RIGHT_HALF);

        buttonMatrixState->groupTwo.rightSingleDose = !digitalRead(IN_GROUP_TWO_SMALL_DRINKS);
        buttonMatrixState->groupTwo.rightDoubleDose = !digitalRead(IN_GROUP_TWO_BIG_DRINKS);

        pinMode(OUT_GROUP_TWO_RIGHT_HALF, INPUT);
        selectScanLine(OUT_TEA_WATER);

        buttonMatrixState->tea = !digitalRead(IN_TEA_WATER);

        pinMode(OUT_TEA_WATER, INPUT);
    }
};

#endif // BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H
