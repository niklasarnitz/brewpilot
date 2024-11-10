//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_BOILERCONTROLLER_H
#define BREWPILOT_BOILERCONTROLLER_H

#include "state/State.h"
#include "HardwareController.h"
#include "Arduino.h"
#include "Config.h"

class BoilerController : public HardwareController {
public:
    void loop(State &state) override;
};


#endif //BREWPILOT_BOILERCONTROLLER_H
