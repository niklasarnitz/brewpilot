#ifndef BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H
#define BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H

#include "MachineButtonMatrixHandler.h"
#include "../../../StaticConfig.h"
#include "../../../utils/noop.h"
#include "Arduino.h"

constexpr uint8_t IN_GROUP_TWO_BIG_DRINKS = FLEXIBLE_IO_1;
constexpr uint8_t IN_GROUP_TWO_SMALL_DRINKS = FLEXIBLE_IO_2;
constexpr uint8_t IN_GROUP_TWO_STOP = FLEXIBLE_IO_3;
constexpr uint8_t IN_GROUP_ONE_BIG_DRINKS = FLEXIBLE_IO_4;
constexpr uint8_t IN_GROUP_ONE_SMALL_DRINKS = FLEXIBLE_IO_5;
constexpr uint8_t IN_GROUP_ONE_STOP = FLEXIBLE_IO_6;

constexpr uint8_t OUT_GROUP_ONE_LEFT_HALF_AND_STOP = FLEXIBLE_IO_7;
constexpr uint8_t OUT_GROUP_ONE_RIGHT_HALF = FLEXIBLE_IO_8;

constexpr uint8_t OUT_TEA_WATER = FLEXIBLE_IO_9;

constexpr uint8_t OUT_GROUP_TWO_LEFT_HALF_AND_STOP = FLEXIBLE_IO_10;
constexpr uint8_t OUT_GROUP_TWO_RIGHT_HALF = FLEXIBLE_IO_11;

constexpr uint8_t IN_TEA_WATER = FLEXIBLE_IO_12;

class LaCimbaliM29ButtonMatrixHandler : public MachineButtonMatrixHandler
{
public:
    LaCimbaliM29ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void initializePins();

    void handle();
};

#endif // BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H
