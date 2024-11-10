//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_GROUPCONTROLLER_H
#define BREWPILOT_GROUPCONTROLLER_H

#include "state/State.h"
#include "HardwareController.h"

class GroupController : public HardwareController {
public:
    void loop(State &state) override {};
};


#endif //BREWPILOT_GROUPCONTROLLER_H
