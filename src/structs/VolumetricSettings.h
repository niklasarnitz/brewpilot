#ifndef BREWPILOT_VOLUMETRICSETTINGS_H
#define BREWPILOT_VOLUMETRICSETTINGS_H

struct VolumetricSettings
{
    unsigned long leftSingleEspressoPulses = 0;
    unsigned long leftDoubleEspressoPulses = 0;
    unsigned long rightSingleEspressoPulses = 0;
    unsigned long rightDoubleEspressoPulses = 0;

    unsigned long teaWaterMilliseconds = 0;
};

#endif // BREWPILOT_VOLUMETRICSETTINGS_H