#include "handler/Web.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>

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
WebServer WebHandler::serverWeb(80);

WebHandler::WebHandler()
{
}

void WebHandler::setup()
{
    serverWeb.serveStatic("/web/js/jquery-min.js", LittleFS, "/web/js/jquery-min.js");
    serverWeb.serveStatic("/web/js/functions.js", LittleFS, "/web/js/functions.js");
    serverWeb.serveStatic("/web/js/bootstrap.min.js", LittleFS, "/web/js/bootstrap.min.js");
    serverWeb.serveStatic("/web/js/bootstrap.min.js.map", LittleFS, "/web/js/bootstrap.min.js.map");
    serverWeb.serveStatic("/web/css/bootstrap.min.css", LittleFS, "/web/css/bootstrap.min.css");
    serverWeb.serveStatic("/web/css/style.css", LittleFS, "/web/css/style.css");
    serverWeb.serveStatic("/web/img/logo.png", LittleFS, "/web/img/logo.png");

    serverWeb.on("/", handleRoot);
    serverWeb.on("/savewifi", HTTP_POST, handleSaveWifiConfig);
    serverWeb.on("/savemqtt", HTTP_POST, handleSaveMQTTConfig);
    serverWeb.on("/saveaquamqtt", HTTP_POST, handleSaveAquaMQTTConfig);
    serverWeb.on("/debug", handleDebug);
    serverWeb.on("/MQTT", handleMQTT);
    serverWeb.on("/Aquamqtt", handleAquaMQTT);
    serverWeb.on("/reboot", handleReboot);
    serverWeb.on("/fsbrowser", handleFSbrowser);
    serverWeb.on("/readFile", handleReadfile);
    serverWeb.onNotFound(handleNotFound);
    serverWeb.begin();
}

void WebHandler::loop()
{
    serverWeb.handleClient();
}

void WebHandler::handleNotFound()
{
    String message = F("File Not Found\n\n");
    message += F("URI: ");
    message += serverWeb.uri();
    message += F("\nMethod: ");
    message += (serverWeb.method() == HTTP_GET) ? "GET" : "POST";
    message += F("\nArguments: ");
    message += serverWeb.args();
    message += F("\n");
    for (uint8_t i = 0; i < serverWeb.args(); i++)
    {
        message += " " + serverWeb.argName(i) + ": " + serverWeb.arg(i) + "\n";
    }
    serverWeb.send(404, F("text/plain"), message);
}

void WebHandler::handleMQTT()
{
    String result;
    result += F("<html>");
    result += FPSTR(aquamqtt::web::HTTP_HEADER);
    result += FPSTR(aquamqtt::web::HTTP_MQTT);
    result += F("</html>");

    result.replace("{{VERSION}}", aquamqtt::VERSION);
    result.replace("{{mqtt_server}}", mqttSettings.mqtt_server);
    result.replace("{{mqtt_port}}", String(mqttSettings.mqtt_port));
    result.replace("{{mqtt_user}}", mqttSettings.mqtt_user);
    result.replace("{{mqtt_password}}", mqttSettings.mqtt_password);
    bool enable = (mqttSettings.enableDiscovery != 0);

    if (enable)
    {
        result.replace("{{discoveryenabled}}", "checked");
    }
    else
    {
        result.replace("{{discoveryenabled}}", "");
    }

    result.replace("{{haDiscoveryPrefix}}", mqttSettings.haDiscoveryPrefix);
    result.replace("{{brokerClientId}}", mqttSettings.brokerClientId);
    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::handleAquaMQTT()
{
    String result;
    result += F("<html>");
    result += FPSTR(aquamqtt::web::HTTP_HEADER);
    result += FPSTR(aquamqtt::web::HTTP_AQUA);
    result += F("</html>");

    result.replace("{{VERSION}}", aquamqtt::VERSION);
    result.replace("{{aqua_heatpump}}", aquamqttSettings.heatpumpModelName);
    switch (aquamqttSettings.operationMode)
    {
        case config::EOperationMode::MITM:
            result.replace("{{SelMITM}}", "selected");
            result.replace("{{SelLIST}}", "");
            break;
        case config::EOperationMode::LISTENER:
            result.replace("{{SelMITM}}", "");
            result.replace("{{SelLIST}}", "selected");
            break;
        default:
            break;
    }

    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::handleRoot()
{
    String result;
    result += F("<html>");
    result += FPSTR(aquamqtt::web::HTTP_HEADER);
    result += FPSTR(aquamqtt::web::HTTP_WIFI);
    result += F("</html>");
    result.replace("{{VERSION}}", aquamqtt::VERSION);
    result.replace("{{ssid}}", String(wifiSettings.ssid));
    result.replace("{{wifipw}}", wifiSettings.password);
    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::handleSaveWifiConfig()
{
    if (!serverWeb.hasArg("WIFISSID"))
    {
        serverWeb.send(500, "text/plain", "BAD ARGS");
        return;
    }

    JsonDocument doc;
    doc["ssid"]      = serverWeb.arg("WIFISSID");
    doc["password"]  = serverWeb.arg("WIFIpassword");
    doc["ipAddress"] = serverWeb.arg("ipAddress");
    doc["ip"]        = serverWeb.arg("ipMask");
    doc["ipGW"]      = serverWeb.arg("ipGW");

    File configFile = LittleFS.open("/config/wifi.json", "w");
    if (!configFile)
    {
        Serial.println("[error] Can't open file wifi.json for writing");
    }
    else
    {
        serializeJson(doc, configFile);
    }
    configFile.close();
    // Reload config to update variable wifiSettings
    if (loadWifiConfig())
    {
        WebHandler::reboot(serverWeb.arg("reboot").toInt() == 1, "/");
    }
    else
    {
        Serial.println("[error] Can't update wifiSettings var");
    }
}

void WebHandler::handleSaveMQTTConfig()
{
    JsonDocument doc;
    doc["mqtt_server"]       = serverWeb.arg("MQTT_Server");
    doc["mqtt_port"]         = serverWeb.arg("MQTT_ServerPort");
    doc["mqtt_user"]         = serverWeb.arg("MQTT_UserName");
    doc["mqtt_password"]     = serverWeb.arg("MQTT_Password");
    doc["haDiscoveryPrefix"] = serverWeb.arg("MQTT_Prefix");
    doc["brokerClientId"]    = serverWeb.arg("MQTT_BrokerID");

    // when value is false, nothing send, so if send =>1; if not => 0
    serverWeb.hasArg("enableDiscovery") ? doc["enableDiscovery"] = 1 : doc["enableDiscovery"] = 0;

    File configFile = LittleFS.open("/config/mqtt.json", "w");
    if (!configFile)
    {
    }
    else
    {
        serializeJson(doc, configFile);
    }
    configFile.close();
    if (loadMqttConfig())
    {
        WebHandler::reboot(serverWeb.arg("reboot").toInt() == 1, "/MQTT");
    }
    else
    {
        Serial.println("[error] Can't update mqttSettings var");
    }
}

void WebHandler::handleSaveAquaMQTTConfig()
{
    JsonDocument doc;
    doc["heatpumpModelName"] = serverWeb.arg("AQUA_HeatPump");
    doc["operationMode"]     = serverWeb.arg("AQUA_Mode").toInt();

    File configFile = LittleFS.open("/config/aquamqtt.json", "w");
    if (!configFile)
    {
        Serial.println("[error] Can't open file aquamqtt.json for writing");
    }
    else
    {
        serializeJson(doc, configFile);
    }
    configFile.close();
    if (loadAquaMqttConfig())
    {
        WebHandler::reboot(serverWeb.arg("reboot").toInt() == 1, "/Aquamqtt");
    }
    else
    {
        Serial.println("[error] Can't update aquamqttSettings var");
    }
}

void WebHandler::handleDebug()
{
    String result;

    result += F("<html>");
    result += FPSTR(aquamqtt::web::HTTP_HEADER);
    result += F("<h1>Tools</h1>");
    result += F("<div class='btn-group-vertical'>");
    result += F("<a href='/fsbrowser' class='btn btn-primary mb-2'>FSbrowser</button>");
    result += F("<a href='/reboot' class='btn btn-primary mb-2'>Reboot</button>");
    result += F("</div></body></html>");
    result.replace("{{VERSION}}", aquamqtt::VERSION);
    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::reboot(bool perform, String newLocation)
{
    serverWeb.sendHeader("Location", newLocation);
    serverWeb.send(303);
    if (perform)
    {
        ESP.restart();
    }
}

void WebHandler::handleReboot()
{
    serverWeb.sendHeader("Location", "/");
    serverWeb.send(303);
    ESP.restart();
}

void WebHandler::handleFSbrowser()
{
    String result;
    result += F("<html>");
    result += FPSTR(aquamqtt::web::HTTP_HEADER);
    result += F("<h1>FSBrowser</h1>");
    result += F("<nav id='navbar-custom' class='navbar navbar-default navbar-fixed-left'>");
    result += F("      <div class='navbar-header'>");
    result += F("        <!--<a class='navbar-brand' href='#'>Brand</a>-->");
    result += F("      </div>");
    result += F("<ul class='nav navbar-nav'>");

    File dir  = LittleFS.open("/config/");
    File file = dir.openNextFile();
    while (file)
    {
        String tmp = file.name();
        result += F("<li><a href='#' onClick=\"readfile('");
        result += tmp;
        result += F("');\">");
        result += tmp;
        result += F(" ( ");
        result += file.size();
        result += F(" o)</a></li>");
        file = dir.openNextFile();
    }
    file.close();
    result += F("</ul></nav>");
    result += F("<div class='container-fluid' >");
    result += F("  <div class='app-main-content'>");
    result += F("<form method='POST' action='saveFile'>");
    result += F("<div class='form-group'>");
    result += F(" <label for='file'>File : <span id='title'></span></label>");
    result += F("<input type='hidden' name='filename' id='filename' value=''>");
    result += F(" <textarea class='form-control' id='file' name='file' rows='10'>");
    result += F("</textarea>");
    result += F("</div>");
    result += F("</Form>");
    result += F("</div>");
    result += F("</div>");
    result += F("</body></html>");

    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::handleReadfile()
{
    String result;
    String filename = "/config/" + serverWeb.arg(0);
    File   file     = LittleFS.open(filename, "r");

    if (!file)
    {
        return;
    }

    while (file.available())
    {
        result += (char) file.read();
    }
    file.close();
    serverWeb.send(200, F("text/html"), result);
}
}  // namespace aquamqtt