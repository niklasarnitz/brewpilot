//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_CONFIG_H
#define BREWPILOT_CONFIG_H

#define DEBUG true

// Pins
// Outputs
#define RELAY_PUMP 18
#define RELAY_GROUP_ONE 5
#define RELAY_GROUP_TWO 4
#define RELAY_BOILER_FILL 2
#define RELAY_TEA 15

// Inputs
// Keyboard Matrix
#define IN_GROUP_TWO_BIG_DRINKS 13
#define IN_GROUP_TWO_SMALL_DRINKS 12
#define IN_GROUP_TWO_STOP 14

#define IN_GROUP_ONE_BIG_DRINKS 27
#define IN_GROUP_ONE_SMALL_DRINKS 26
#define IN_GROUP_ONE_STOP 25

#define OUT_GROUP_ONE_LEFT_HALF_AND_STOP 33
#define OUT_GROUP_ONE_RIGHT_HALF 32

#define OUT_TEA_WATER 19

#define OUT_GROUP_TWO_LEFT_HALF_AND_STOP 21
#define OUT_GROUP_TWO_RIGHT_HALF 22

#define IN_TEA_WATER 23

// Boiler Probe
#define BOILER_PROBE_PIN 34
#define OUT_BOILER_VOLTAGE 17

// Flow Meters
#define GROUP_ONE_FLOW_METER_PIN 16
#define GROUP_TWO_FLOW_METER_PIN 35

// Boiler Stuff
#define BOILER_FILL_LEVEL 1000 // Value for the LaCimbali M29 Select DT2 / R

#endif // BREWPILOT_CONFIG_H
