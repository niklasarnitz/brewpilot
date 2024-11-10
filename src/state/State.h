//
// Created by Niklas Arnitz on 10.11.24.
//

#ifndef BREWPILOT_STATE_H
#define BREWPILOT_STATE_H

enum BoilerState {
    BOILER_FILLED,
    BOILER_FILLING,
    BOILER_BELOW_TARGET,
    BOILER_UNKNOWN
};

enum GroupState {
    GROUP_IDLE,
    GROUP_EXTRACTING
};

enum TeaWaterState {
    TEA_WATER_IDLE,
    TEA_WATER_EXTRACTING
};

enum PumpState {
    PUMP_IDLE,
    PUMP_RUNNING
};

class State {
private:
    BoilerState boilerState;
    GroupState groupOneState;
    GroupState groupTwoState;
    TeaWaterState teaWaterState;
    PumpState pumpState;
public:
    State() {
        boilerState = BOILER_UNKNOWN;
        groupOneState = GROUP_IDLE;
        groupTwoState = GROUP_IDLE;
        teaWaterState = TEA_WATER_IDLE;
        pumpState = PUMP_IDLE;
    };

    void setBoilerState(BoilerState newState);

    void setGroupOneState(GroupState newState);

    void setGroupTwoState(GroupState newState);

    void setTeaWaterState(TeaWaterState newState);

    void setPumpState(PumpState newState);

    BoilerState getBoilerState();

    GroupState getGroupOneState();

    GroupState getGroupTwoState();

    TeaWaterState getTeaWaterState();

    PumpState getPumpState();
};


#endif //BREWPILOT_STATE_H
