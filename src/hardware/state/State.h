//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_STATE_H
#define BREWPILOT_STATE_H

struct State
{
    bool isFillingBoiler = false;

    bool isExtractingTeaWater = false;

    bool groupOneIsExtracting = false;

#ifdef MACHINE_HAS_DUAL_GROUP
    bool groupTwoIsExtracting = false;
#endif

    bool isInProgrammingMode = false;
};

#endif // BREWPILOT_STATE_H
