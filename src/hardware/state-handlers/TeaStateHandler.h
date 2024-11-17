//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_TEASTATEHANDLER_H
#define BREWPILOT_TEASTATEHANDLER_H

#include "GenericStateHandler.h"
#include "Arduino.h"

class TeaStateHandler : public GenericStateHandler {
private:
    bool &teaButtonPressed;
    bool &isExtractingTeaWater;
    unsigned long startTime;
public:
    TeaStateHandler(bool &teaButtonPressed, bool &isExtractingTeaWater) : teaButtonPressed(teaButtonPressed),
                                                                          isExtractingTeaWater(isExtractingTeaWater) {
        startTime = 0;
    }

    void handleState() override {
        if (isExtractingTeaWater) {
            // TODO: Implement to read this from the FS + programming
            if (millis() - startTime > 2000) {
                isExtractingTeaWater = false;
            }
        } else if (teaButtonPressed) {
            isExtractingTeaWater = true;
            startTime = millis();
        }
    }
};

#endif //BREWPILOT_TEASTATEHANDLER_H
