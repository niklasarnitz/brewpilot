#include "WebServer.h"
#include <ArduinoJson.h>

WebServer::WebServer(State& state) : server(80), state(state) {}

void WebServer::begin() {
    if(!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }
    
    serveStaticFiles();
    setupApiRoutes();
    
    server.begin();
}

void WebServer::serveStaticFiles() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
    });
}

void WebServer::setupApiRoutes() {
    server.on("/api/state", HTTP_GET, [this](AsyncWebServerRequest *request){
        StaticJsonDocument<400> doc;
        
        // Add state information to JSON
        doc['groupOneIsExtracting'] = state.groupOneIsExtracting;
        doc['groupTwoIsExtracting'] = state.groupTwoIsExtracting;
        doc['isFillingBoiler'] = state.isFillingBoiler;
        doc['isExtractingTeaWater'] = state.isExtractingTeaWater;
        
        String response;
        serializeJson(doc, response);
        
        request->send(200, "application/json", response);
    });
} 