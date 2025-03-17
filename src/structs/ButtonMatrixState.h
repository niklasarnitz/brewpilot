//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_BUTTONMATRIXSTATE_H
#define BREWPILOT_BUTTONMATRIXSTATE_H

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

#endif // BREWPILOT_BUTTONMATRIXSTATE_H
