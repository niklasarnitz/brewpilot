#include "State.h"

void State::setBoilerState(BoilerState newState) {
    boilerState = newState;
}

void State::setGroupOneState(GroupState newState) {
    groupOneState = newState;
}

void State::setGroupTwoState(GroupState newState) {
    groupTwoState = newState;
}

void State::setTeaWaterState(TeaWaterState newState) {
    teaWaterState = newState;
}

void State::setPumpState(PumpState newState) {
    pumpState = newState;
}

BoilerState State::getBoilerState() {
    return boilerState;
}

GroupState State::getGroupOneState() {
    return groupOneState;
}

GroupState State::getGroupTwoState() {
    return groupTwoState;
}

TeaWaterState State::getTeaWaterState() {
    return teaWaterState;
}

PumpState State::getPumpState() {
    return pumpState;
}