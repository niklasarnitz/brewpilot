#include <Arduino.h>
#include "controllers/EspressoController.h"
#include "state/State.h"

EspressoController espressoController;

State state;

void setup() {
}

void loop() {
    espressoController.loop(state);
    delay(10);
}