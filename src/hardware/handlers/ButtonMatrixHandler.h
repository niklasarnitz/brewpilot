//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_BUTTONMATRIXHANDLER_H
#define BREWPILOT_BUTTONMATRIXHANDLER_H

#include "structs/ButtonMatrixState.h"

class ButtonMatrixHandler {
private:
    ButtonMatrixState &buttonMatrixState;

public:
    ButtonMatrixHandler(ButtonMatrixState &buttonMatrixState) : buttonMatrixState(buttonMatrixState) {};
};

#endif //BREWPILOT_BUTTONMATRIXHANDLER_H
