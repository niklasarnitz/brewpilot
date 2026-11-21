#include "input/matrix/LaCimbaliM29ButtonMatrixHandler.h"

#include <Arduino.h>

#include "config/machines/LaCimbaliM29Select.h"
#include "utils/noop.h"

using namespace la_cimbali_m29_select::panel;

LaCimbaliM29ButtonMatrixHandler::LaCimbaliM29ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState) : buttonMatrixState(buttonMatrixState) {}

void LaCimbaliM29ButtonMatrixHandler::selectScanLine(uint8_t pin)
{
    // Preload LOW before enabling the output so it cannot pulse HIGH.
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    noop();
}

void LaCimbaliM29ButtonMatrixHandler::initializePins()
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

void LaCimbaliM29ButtonMatrixHandler::handle()
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
