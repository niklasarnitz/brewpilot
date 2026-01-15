//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_CONFIG_H
#define BREWPILOT_CONFIG_H

constexpr bool DEBUG = true;

// Pins
// Outputs
constexpr uint8_t RELAY_PUMP = 18;
constexpr uint8_t RELAY_GROUP_ONE = 5;
constexpr uint8_t RELAY_GROUP_TWO = 4;
constexpr uint8_t RELAY_BOILER_FILL = 2;
constexpr uint8_t RELAY_TEA = 15;

// Inputs
// Keyboard Matrix
constexpr uint8_t FLEXIBLE_IO_1 = 13;
constexpr uint8_t FLEXIBLE_IO_2 = 12;
constexpr uint8_t FLEXIBLE_IO_3 = 14;
constexpr uint8_t FLEXIBLE_IO_4 = 27;
constexpr uint8_t FLEXIBLE_IO_5 = 26;
constexpr uint8_t FLEXIBLE_IO_6 = 25;
constexpr uint8_t FLEXIBLE_IO_7 = 33;
constexpr uint8_t FLEXIBLE_IO_8 = 32;
constexpr uint8_t FLEXIBLE_IO_9 = 19;
constexpr uint8_t FLEXIBLE_IO_10 = 21;
constexpr uint8_t FLEXIBLE_IO_11 = 22;
constexpr uint8_t FLEXIBLE_IO_12 = 23;

// Boiler Probe
constexpr uint8_t BOILER_PROBE_PIN = 34;
constexpr uint8_t OUT_BOILER_VOLTAGE = 17;

// Flow Meters
constexpr uint8_t GROUP_ONE_FLOW_METER_PIN = 16;
// TODO: Move 35 to a pin with internall pull up resitor once we're using the bigger esp32 board
constexpr uint8_t GROUP_TWO_FLOW_METER_PIN = 35;

// Boiler Stuff
constexpr uint16_t BOILER_FILL_LEVEL = 1000; // Value for the LaCimbali M29 Select DT2 / R

#endif // BREWPILOT_CONFIG_H
