#pragma once

struct GroupButtonMatrixState
{
    bool leftSingleDose = false;
    bool leftDoubleDose = false;
    bool continuous = false;
    bool rightSingleDose = false;
    bool rightDoubleDose = false;
};

struct ButtonMatrixState
{
    GroupButtonMatrixState groupOne{};
    GroupButtonMatrixState groupTwo{};
    bool tea = false;
};
