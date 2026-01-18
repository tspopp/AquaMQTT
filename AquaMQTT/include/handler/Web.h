#ifndef AQUAMQTT_WEB_H
#define AQUAMQTT_WEB_H
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "config/config.h"

namespace aquamqtt
{
namespace apiconfig
{
enum EApiEndpoint
{
    WIFI,
    MQTT,
    AQUA
};
}  // namespace apiconfig

class WebHandler
{
public:
    WebHandler();
    void setup();
    void loop();

private:
    static void handleRoot(AsyncWebServerRequest* request);
    static void handleNotFound(AsyncWebServerRequest* request);
    // Get handlers
    static void handleWifiGet(AsyncWebServerRequest* request);
    static void handleMQTTGet(AsyncWebServerRequest* request);
    static void handleAquaGet(AsyncWebServerRequest* request);
    static void handleGet(AsyncWebServerRequest* request, apiconfig::EApiEndpoint endpoint);
    // Post handler
    static void handleWifiPost(AsyncWebServerRequest* request);
    static void handleMQTTPost(AsyncWebServerRequest* request);
    static void handleAquaMQTTPost(AsyncWebServerRequest* request);

    // utility handlers
    static bool handleDataSave(apiconfig::EApiEndpoint endpoint, JsonObject data);

    static AsyncWebServer serverWeb;
};

}  // namespace aquamqtt
#endif  // AQUAMQTT_WEB_H