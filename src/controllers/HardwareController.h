//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_HARDWARECONTROLLER_H
#define BREWPILOT_HARDWARECONTROLLER_H


class HardwareController {
public:
    virtual void loop() = 0;

protected:
    HardwareController() {};
};


#endif //BREWPILOT_HARDWARECONTROLLER_H
