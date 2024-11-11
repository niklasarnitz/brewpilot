//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_INPUTHANDLER_H
#define BREWPILOT_INPUTHANDLER_H

#include "hardware/devices/Button.h"


class InputHandler {
private:
//    TODO: Implement the reading of the button matrix here using coordinates
    Button buttons[9] = {
            // GROUP_ONE
            Button(0),
            Button(1),
            Button(2),
            Button(3),
//            TEA
            Button(4),
//            GROUP_TWO
            Button(5),
            Button(6),
            Button(7),
            Button(8)
    };

public:
    void readInputs();
};


#endif //BREWPILOT_INPUTHANDLER_H
