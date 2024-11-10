//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_TEAWATERCONTROLLER_H
#define BREWPILOT_TEAWATERCONTROLLER_H

#include "HardwareController.h"
#include "state/State.h"

class TeaWaterController : public HardwareController {
public:
    void loop(State &state) override {};
};


#endif //BREWPILOT_TEAWATERCONTROLLER_H
