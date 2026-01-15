#ifndef BREWPILOT_RANCILIOS27BUTTONMATRIXHANDLER_H
#define BREWPILOT_RANCILIOS27BUTTONMATRIXHANDLER_H

#include "MachineButtonMatrixHandler.h"
#include "../../../StaticConfig.h"
#include "../../../utils/noop.h"
#include "Arduino.h"

constexpr uint8_t OUT_DOSE_BUTTONS = FLEXIBLE_IO_7;
constexpr uint8_t OUT_TEA_WATER = FLEXIBLE_IO_9;

constexpr uint8_t IN_DOSE_1 = FLEXIBLE_IO_4;
constexpr uint8_t IN_DOSE_2 = FLEXIBLE_IO_5;
constexpr uint8_t IN_DOSE_3 = FLEXIBLE_IO_6;
constexpr uint8_t IN_DOSE_4 = FLEXIBLE_IO_1;
constexpr uint8_t IN_CONTINUOUS = FLEXIBLE_IO_2;
constexpr uint8_t IN_TEA_WATER = FLEXIBLE_IO_12;

class RancilioS27ButtonMatrixHandler : public MachineButtonMatrixHandler
{
public:
    RancilioS27ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

    void initializePins();

    void handle();
};

#endif // BREWPILOT_RANCILIOS27BUTTONMATRIXHANDLER_H
