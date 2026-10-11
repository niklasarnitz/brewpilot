//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_STATE_H
#define BREWPILOT_STATE_H

enum class ProgrammingTarget
{
    NONE,
    SELECTING,
    GROUP_ONE,
    GROUP_TWO,
    TEA
};

struct State
{
    bool isFillingBoiler = false;
    bool boilerFillFault = false;
    bool groupOneIsExtracting = false;
    bool groupTwoIsExtracting = false;
    bool isExtractingTeaWater = false;

    ProgrammingTarget programmingTarget = ProgrammingTarget::NONE;
};

#endif // BREWPILOT_STATE_H
