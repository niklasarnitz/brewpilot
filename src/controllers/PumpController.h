//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_PUMPCONTROLLER_H
#define BREWPILOT_PUMPCONTROLLER_H

#include "state/State.h"
#include "HardwareController.h"

class PumpController : public HardwareController {
public:
    void loop(State &state) override {};
};


#endif //BREWPILOT_PUMPCONTROLLER_H
