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
#define FLEXIBLE_IO_1 13
#define FLEXIBLE_IO_2 12
#define FLEXIBLE_IO_3 14
#define FLEXIBLE_IO_4 27
#define FLEXIBLE_IO_5 26
#define FLEXIBLE_IO_6 25
#define FLEXIBLE_IO_7 33
#define FLEXIBLE_IO_8 32
#define FLEXIBLE_IO_9 19
#define FLEXIBLE_IO_10 21
#define FLEXIBLE_IO_11 22
#define FLEXIBLE_IO_12 23

// Boiler Probe
#define BOILER_PROBE_PIN 34
#define OUT_BOILER_VOLTAGE 17

// Flow Meters
#define GROUP_ONE_FLOW_METER_PIN 16
// TODO: Move 35 to a pin with internall pull up resitor once we're using the bigger esp32 board
#define GROUP_TWO_FLOW_METER_PIN 35

// Boiler Stuff
#define BOILER_FILL_LEVEL 1000 // Value for the LaCimbali M29 Select DT2 / R

#endif // BREWPILOT_CONFIG_H
