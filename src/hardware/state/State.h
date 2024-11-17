//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_STATE_H
#define BREWPILOT_STATE_H

struct State {
    bool isFillingBoiler = false;

    bool isExtractingTeaWater = false;

    bool groupOneIsExtracting = false;
    bool groupTwoIsExtracting = false;
};

#endif //BREWPILOT_STATE_H
