#include <Arduino.h>
#include "controllers/EspressoController.h"

EspressoController espressoController;

void setup() {
}

void loop() {
    espressoController.loop();
}