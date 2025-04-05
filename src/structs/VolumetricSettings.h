#ifndef BREWPILOT_TARGETFLOWMETERPULSES_H
#define BREWPILOT_TARGETFLOWMETERPULSES_H

struct VolumetricSettings
{
    long leftSingleEspressoPulses = 0;
    long leftDoubleEspressoPulses = 0;
    long rightSingleEspressoPulses = 0;
    long rightDoubleEspressoPulses = 0;

    long teaWaterMilliseconds = 0;
};

#endif // BREWPILOT_TARGETFLOWMETERPULSES_H