#include "handler/Web.h"

#include <Arduino.h>
#include <AsyncJson.h>
#include <AsyncMessagePack.h>

#include "LittleFS.h"
#include "Version.h"
#include "config/Configuration.h"
#include "config/Modes.h"
#include "config/config.h"
extern aquamqtt::MqttSettingStruct mqttSettings;
extern aquamqtt::WifiConfigStruct  wifiSettings;
extern aquamqtt::AquaMqttStruct    aquamqttSettings;

namespace aquamqtt
{
AsyncWebServer                      WebHandler::serverWeb(80);
static AsyncCallbackJsonWebHandler* handleWifi     = new AsyncCallbackJsonWebHandler("/api/wifi");
static AsyncCallbackJsonWebHandler* handleMqtt     = new AsyncCallbackJsonWebHandler("/api/mqtt");
static AsyncCallbackJsonWebHandler* handleAquamqtt = new AsyncCallbackJsonWebHandler("/api/aquamqtt");

WebHandler::WebHandler()
{
}

void WebHandler::setup()
{
    // Serve all static files from "web" directory
    serverWeb.serveStatic("/web/", LittleFS, "/web/");
    // Root handler
    serverWeb.on("/", HTTP_GET, handleRoot);

    // GET api methods handler
    serverWeb.on("/api/wifi", HTTP_GET, handleWifiGet);
    serverWeb.on("/api/mqtt", HTTP_GET, handleMQTTGet);
    serverWeb.on("/api/aqua", HTTP_GET, handleAquaGet);

 

    serverWeb.onNotFound(handleNotFound);
    // DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");  // FIXME for debug only

    // Post api handlers
    handleWifi->setMethod(HTTP_POST);
    handleWifi->onRequest([](AsyncWebServerRequest* request, JsonVariant& json) {
        JsonObject jsonObj = json.as<JsonObject>();

        bool       reboot = jsonObj["reboot"].as<bool>();
        JsonObject data   = jsonObj["data"].as<JsonObject>();

        if (data["ssid"] == "")
        {
            AsyncWebServerResponse* response = request->beginResponse(500, "text/plain", "{message:'empty ssid'}");
            response->addHeader("Content-Type", "application/json");
            request->send(response);
        }
        else
        {

            if (WebHandler::handleDataSave(apiconfig::WIFI, data))
            {
                AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "{status:'OK'}");
                response->addHeader("Content-Type", "application/json");
                request->send(response);
            }
            else
            {
                request->send(500);
            }
        }
        if (reboot)
        {
            delay(2500);  // wait so that web client can see the result
            ESP.restart();
        }
    });
    handleMqtt->setMethod(HTTP_POST);
    handleMqtt->onRequest([](AsyncWebServerRequest* request, JsonVariant& json) {
        JsonObject jsonObj = json.as<JsonObject>();
        JsonObject data    = jsonObj["data"].as<JsonObject>();
        bool       reboot  = jsonObj["reboot"].as<bool>();
        if (WebHandler::handleDataSave(apiconfig::MQTT, data))
        {
            AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "{status:'OK'}");
            response->addHeader("Content-Type", "application/json");
            request->send(response);
        }
        else
        {
            request->send(500);
        }
        if (reboot)
        {
            delay(2500);  // wait so that web client can see the result
            ESP.restart();
        }
    });
    handleAquamqtt->setMethod(HTTP_POST);
    handleAquamqtt->onRequest([](AsyncWebServerRequest* request, JsonVariant& json) {
        JsonObject jsonObj = json.as<JsonObject>();
        JsonObject data    = jsonObj["data"].as<JsonObject>();

        bool reboot = jsonObj["reboot"].as<bool>();
        if (WebHandler::handleDataSave(apiconfig::AQUA, data))
        {
            AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "{'status':'OK'}");
            response->addHeader("Content-Type", "application/json");
            request->send(response);
        }
        else
        {
            request->send(500);
        }
        if (reboot)
        {
            delay(2500);  // wait so that web client can see the result
            ESP.restart();
        }
    });

    serverWeb.addHandler(handleWifi);
    serverWeb.addHandler(handleMqtt);
    serverWeb.addHandler(handleAquamqtt);
    serverWeb.begin();
}

bool WebHandler::handleDataSave(apiconfig::EApiEndpoint endpoint, JsonObject data)
{
    String file;
    switch (endpoint)
    {
        case apiconfig::EApiEndpoint::WIFI:
            file = "wifi";
            break;
        case apiconfig::EApiEndpoint::MQTT:
            file = "mqtt";
            break;
        case apiconfig::EApiEndpoint::AQUA:
            file = "aquamqtt";
            break;
        default:
            break;
    }

    File configFile = LittleFS.open("/config/" + file + ".json", "w");
    if (!configFile)
    {
        Serial.println("[error] Can't open file for writing");
        return false;
    }
    else
    {
        serializeJson(data, configFile);
    }
    configFile.close();
    return true;
}

void WebHandler::loop()
{
}

void WebHandler::handleNotFound(AsyncWebServerRequest* request)
{
    String message = F("File Not Found\n\n");
    if (request->method()
        == HTTP_OPTIONS)  // necessary with allow-request origin "*""
                          // https://github.com/ESP32Async/ESPAsyncWebServer/wiki#adding-default-headers
    {
        request->send(200);
    }
    else
    {
        Serial.println("*********** Request not handled");

        Serial.println(request->url());
        Serial.println(request->method());
        int headers = request->headers();

        for (int i = 0; i < headers; i++)
        {
            const AsyncWebHeader* h = request->getHeader(i);
            Serial.printf("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
        }
        request->send(404, F("text/plain"), message);
    }
}

void WebHandler::handleWifiGet(AsyncWebServerRequest* request)
{
    WebHandler::handleGet(request, apiconfig::EApiEndpoint::WIFI);
}

void WebHandler::handleMQTTGet(AsyncWebServerRequest* request)
{
    WebHandler::handleGet(request, apiconfig::EApiEndpoint::MQTT);
}

void WebHandler::handleAquaGet(AsyncWebServerRequest* request)
{
    WebHandler::handleGet(request, apiconfig::EApiEndpoint::AQUA);
}

void WebHandler::handleGet(AsyncWebServerRequest* request, apiconfig::EApiEndpoint endpoint)
{
    String file;
    switch (endpoint)
    {
        case apiconfig::EApiEndpoint::WIFI:
            file = "wifi";
            break;
        case apiconfig::EApiEndpoint::MQTT:
            file = "mqtt";
            break;
        case apiconfig::EApiEndpoint::AQUA:
            file = "aquamqtt";
            break;
        default:
            break;
    }
    // send content of json file
    AsyncWebServerResponse* response = request->beginResponse(
            LittleFS,
            "/config/" + file + ".json",
            "application/json");
    request->send(response);
}

String versionProcessor(const String& var)
{
    return aquamqtt::VERSION;
}

void WebHandler::handleRoot(AsyncWebServerRequest* request)
{
    request->send(LittleFS, "/index.html", String(), false, versionProcessor);
}

}  // namespace aquamqtt