//
// Created by Niklas Arnitz on 08.11.24.
//

#ifndef BREWPILOT_CONFIG_H
#define BREWPILOT_CONFIG_H

// Pin Configuration
#define BOILER_LEVEL_SENSOR_PIN A0

// TODO: Make this value configurable via the web interface
// Value of the "Full" State of the Boiler Sensor
#define BOILER_FILL_LEVEL 2000

// Number of consecutive reads of the Boiler Sensor that must be below the "Full" Level
#define FILL_TALLY_LIMIT 10


#endif //BREWPILOT_CONFIG_H
