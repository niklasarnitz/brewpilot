#ifndef BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H
#define BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H

#include "../../../StaticConfig.h"
#include "MachineButtonMatrixHandler.h"
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

namespace LaCimbali
{
    constexpr uint8_t OUT_TEA_WATER = FLEXIBLE_IO_9;
    constexpr uint8_t IN_TEA_WATER = FLEXIBLE_IO_12;
}

constexpr uint8_t OUT_GROUP_TWO_LEFT_HALF_AND_STOP = FLEXIBLE_IO_10;
constexpr uint8_t OUT_GROUP_TWO_RIGHT_HALF = FLEXIBLE_IO_11;

class LaCimbaliM29ButtonMatrixHandler : public MachineButtonMatrixHandler
{
public:
    explicit LaCimbaliM29ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void initializePins() override;

    void handle() override;
};

#endif // BREWPILOT_LACIMBALIM29BUTTONMATRIXHANDLER_H
