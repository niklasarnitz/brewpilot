#include <Arduino.h>
#include "hardware/controllers/HardwareController.h"

#include "Config.h"

HardwareController hardwareController;

void groupOneFlowMeterHandler() {
    hardwareController.updateFlow(GROUP_ONE_ID);
}

void groupTwoFlowMeterHandler() {
    hardwareController.updateFlow(GROUP_TWO_ID);
}

void setup() {
    attachInterrupt(digitalPinToInterrupt(GROUP_ONE_FLOW_METER_PIN), groupOneFlowMeterHandler, CHANGE);
    attachInterrupt(digitalPinToInterrupt(GROUP_TWO_FLOW_METER_PIN), groupTwoFlowMeterHandler, CHANGE);
}

void loop() {
    hardwareController.loop();
    delay(10);
}