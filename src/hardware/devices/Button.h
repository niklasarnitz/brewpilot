//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_BUTTON_H
#define BREWPILOT_BUTTON_H

class Button {
private:
    int pin;

public:
    explicit Button(int pin) : pin(pin) {};
};


#endif //BREWPILOT_BUTTON_H
