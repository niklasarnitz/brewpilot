#pragma once

enum class ProgrammingTarget
{
    NONE,
    GROUP_ONE,
    GROUP_TWO,
    TEA
};

struct State
{
    bool isFillingBoiler = false;
    bool groupOneIsExtracting = false;
    bool groupTwoIsExtracting = false;
    bool isExtractingTeaWater = false;

    ProgrammingTarget programmingTarget = ProgrammingTarget::NONE;
};
