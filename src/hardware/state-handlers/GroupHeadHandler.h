//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_GROUPHEADHANDLER_H
#define BREWPILOT_GROUPHEADHANDLER_H

#include "GenericStateHandler.h"
#include "hardware/enums/GroupHeadButtonEvent.h"

#include <climits>

class GroupHeadStateHandler : public GenericStateHandler
{
    GroupHeadButtonEvent &event;
    bool &isExtracting;

    long remainingPulses;

    long getRemainingPulses()
    {
        // TODO: Get this from memory/FS
        switch (event)
        {
        case LEFT_SINGLE_ESPRESSO:
            return 0;
        case RIGHT_SINGLE_ESPRESSO:
            return 1;
        case LEFT_DOUBLE_ESPRESSO:
            return 2;
        case RIGHT_DOUBLE_ESPRESSO:
            return 3;
        case CONTINUOUS:
            return LONG_MAX;
        default:
            return 0;
        };
    }

public:
    GroupHeadStateHandler(bool &isExtracting, GroupHeadButtonEvent &event) : isExtracting(isExtracting),
                                                                             event(event)
    {
        remainingPulses = 0;
    }

    void handleState() override
    {
        if (isExtracting)
        {
            if (event == CONTINUOUS)
            {
                isExtracting = false;
                remainingPulses = 0;
            }
        }
        else if (event != NONE)
        {
            isExtracting = true;

            remainingPulses = getRemainingPulses();
        }

        if (remainingPulses == 0)
        {
            isExtracting = false;
        }

        // TODO: Implement "hold to program" handling
        // TODO: Implement programming of the doses
    }

    void flowMeterPulseInterrupt()
    {
        if (remainingPulses > 0)
        {
            remainingPulses--;
        }
    }
};

#endif // BREWPILOT_GROUPHEADHANDLER_H
