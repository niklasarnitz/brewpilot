#pragma once

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "../hardware/state/State.h"

class WebServer {
private:
    AsyncWebServer server;
    State& state;

public:
    WebServer(State& state);
    void begin();
    void serveStaticFiles();
    void setupApiRoutes();
}; 