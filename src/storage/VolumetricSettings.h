#pragma once

#include <stdint.h>

struct VolumetricSettings
{
    uint32_t leftSingleEspressoPulses = 0;
    uint32_t leftDoubleEspressoPulses = 0;
    uint32_t rightSingleEspressoPulses = 0;
    uint32_t rightDoubleEspressoPulses = 0;

    uint32_t teaWaterMilliseconds = 0;
};
