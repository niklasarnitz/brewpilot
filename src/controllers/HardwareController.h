//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_HARDWARECONTROLLER_H
#define BREWPILOT_HARDWARECONTROLLER_H

#include "state/State.h"

class HardwareController {
public:
    virtual void loop(State &state) = 0;

protected:
    HardwareController() {};
};


#endif //BREWPILOT_HARDWARECONTROLLER_H
