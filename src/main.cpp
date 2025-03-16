#include <Arduino.h>
#include "structs/ButtonEvent.h"
#include "Config.h"
#include "hardware/state-handlers/StateHandler.h"
#include "hardware/state/State.h"
#include "hardware/actors/StateActor.h"
#include "hardware/handlers/InputHandler.h"
#include "web/WebServer.h"

ButtonEvent buttonEvent;
State state;
StateHandler stateHandler(state, buttonEvent);
StateActor stateActor(state);
InputHandler inputHandler(buttonEvent);
WebServer webServer(state);

TaskHandle_t webServerTask;

void webServerLoop(void * parameter) {
    webServer.begin();
    for(;;) {
        delay(10);
    }
}

void IRAM_ATTR groupOneFlowMeterHandler() {
    stateHandler.groupOneFlowMeterPulseInterrupt();
}

void IRAM_ATTR groupTwoFlowMeterHandler() {
    stateHandler.groupTwoFlowMeterPulseInterrupt();
}

void setup() {
    Serial.begin(115200);
    
    pinMode(GROUP_ONE_FLOW_METER_PIN, INPUT);
    pinMode(GROUP_TWO_FLOW_METER_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(GROUP_ONE_FLOW_METER_PIN), groupOneFlowMeterHandler, CHANGE);
    attachInterrupt(digitalPinToInterrupt(GROUP_TWO_FLOW_METER_PIN), groupTwoFlowMeterHandler, CHANGE);

    // Create web server task on Core 0
    xTaskCreatePinnedToCore(
        webServerLoop,    // Function to implement the task
        "WebServer",      // Name of the task
        16384,           // Stack size in words (64KB)
        NULL,            // Task input parameter
        1,              // Priority of the task
        &webServerTask,  // Task handle
        0               // Core where the task should run
    );
}

void loop() {
    inputHandler.readInputs();
    stateHandler.handleState();
    stateActor.loop();
}