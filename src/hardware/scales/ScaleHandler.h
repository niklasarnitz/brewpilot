#ifndef BREWPILOT_SCALEHANDLER_H
#define BREWPILOT_SCALEHANDLER_H

#include <AcaiaArduinoBLE.h>

class ScaleHandler
{
private:
    AcaiaArduinoBLE scale = AcaiaArduinoBLE(false);
    float currentWeight = 0;

public:
    explicit ScaleHandler() {};

    void handleScale()
    {
        // Connect to scale
        if (!scale.isConnected())
        {
            scale.init();
            currentWeight = 0;
            if (scale.isConnected())
            {
                Serial.println("Scale connected");
            }
            else
            {
                return;
            }
        }
        else if (scale.heartbeatRequired())
        {
            scale.heartbeat();
        }

        // always call newWeightAvailable to actually receive the datapoint from the scale,
        // otherwise getWeight() will return stale data
        if (scale.newWeightAvailable())
        {
            float newWeight = scale.getWeight();
            if (currentWeight != newWeight) {
                Serial.printf("New weight value: %.1f\n", newWeight);
            }
            currentWeight = newWeight;
        }
    }
};

#endif // BREWPILOT_SCALEHANDLER_H