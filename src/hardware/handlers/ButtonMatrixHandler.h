//
// Created by Niklas Arnitz on 17.11.24.
//

#include "../../structs/ButtonMatrixState.h"
#include "../../utils/noop.h"

#ifndef BREWPILOT_BUTTONMATRIXHANDLER_H
#define BREWPILOT_BUTTONMATRIXHANDLER_H

class ButtonMatrixHandler
{
private:
    ButtonMatrixState *buttonMatrixState;

public:
    ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState) : buttonMatrixState(buttonMatrixState)
    {
        pinMode(OUT_GROUP_ONE_LEFT_HALF_AND_STOP, OUTPUT);
        pinMode(OUT_GROUP_ONE_RIGHT_HALF, OUTPUT);
        pinMode(OUT_GROUP_TWO_LEFT_HALF_AND_STOP, OUTPUT);
        pinMode(OUT_GROUP_TWO_RIGHT_HALF, OUTPUT);
        pinMode(OUT_TEA_WATER, OUTPUT);

        pinMode(IN_GROUP_ONE_BIG_DRINKS, INPUT_PULLUP);
        pinMode(IN_GROUP_ONE_SMALL_DRINKS, INPUT_PULLUP);
        pinMode(IN_GROUP_ONE_STOP, INPUT_PULLUP);

        pinMode(IN_GROUP_TWO_BIG_DRINKS, INPUT_PULLUP);
        pinMode(IN_GROUP_TWO_SMALL_DRINKS, INPUT_PULLUP);
        pinMode(IN_GROUP_TWO_STOP, INPUT_PULLUP);

        pinMode(IN_TEA_WATER, INPUT_PULLUP);

        digitalWrite(OUT_GROUP_ONE_LEFT_HALF_AND_STOP, HIGH);
        digitalWrite(OUT_GROUP_ONE_RIGHT_HALF, HIGH);
        digitalWrite(OUT_GROUP_TWO_LEFT_HALF_AND_STOP, HIGH);
        digitalWrite(OUT_GROUP_TWO_RIGHT_HALF, HIGH);
        digitalWrite(OUT_TEA_WATER, HIGH);
    };

    void handle()
    {
        // TODO: Implement handling for other machines!
        digitalWrite(OUT_GROUP_ONE_LEFT_HALF_AND_STOP, LOW);
        noop();

        buttonMatrixState->groupOne.leftSingleDose = !digitalRead(IN_GROUP_ONE_SMALL_DRINKS);
        buttonMatrixState->groupOne.leftDoubleDose = !digitalRead(IN_GROUP_ONE_BIG_DRINKS);
        buttonMatrixState->groupOne.continuous = !digitalRead(IN_GROUP_ONE_STOP);

        digitalWrite(OUT_GROUP_ONE_LEFT_HALF_AND_STOP, HIGH);
        digitalWrite(OUT_GROUP_ONE_RIGHT_HALF, LOW);
        noop();

        buttonMatrixState->groupOne.rightSingleDose = !digitalRead(IN_GROUP_ONE_SMALL_DRINKS);
        buttonMatrixState->groupOne.rightDoubleDose = !digitalRead(IN_GROUP_ONE_BIG_DRINKS);

        digitalWrite(OUT_GROUP_ONE_RIGHT_HALF, HIGH);
        digitalWrite(OUT_GROUP_TWO_LEFT_HALF_AND_STOP, LOW);
        noop();

        buttonMatrixState->groupTwo.leftSingleDose = !digitalRead(IN_GROUP_TWO_SMALL_DRINKS);
        buttonMatrixState->groupTwo.leftDoubleDose = !digitalRead(IN_GROUP_TWO_BIG_DRINKS);
        buttonMatrixState->groupTwo.continuous = !digitalRead(IN_GROUP_TWO_STOP);

        digitalWrite(OUT_GROUP_TWO_LEFT_HALF_AND_STOP, HIGH);
        digitalWrite(OUT_GROUP_TWO_RIGHT_HALF, LOW);
        noop();

        buttonMatrixState->groupTwo.rightSingleDose = !digitalRead(IN_GROUP_TWO_SMALL_DRINKS);
        buttonMatrixState->groupTwo.rightDoubleDose = !digitalRead(IN_GROUP_TWO_BIG_DRINKS);

        digitalWrite(OUT_GROUP_TWO_RIGHT_HALF, HIGH);
        digitalWrite(OUT_TEA_WATER, LOW);
        noop();

        buttonMatrixState->tea = !digitalRead(IN_TEA_WATER);

        digitalWrite(OUT_TEA_WATER, HIGH);
    };
};

#endif // BREWPILOT_BUTTONMATRIXHANDLER_H
