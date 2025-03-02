#include "handler/Web.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>

#include "LittleFS.h"
#include "config/Configuration.h"
#include "config/config.h"
extern aquamqtt::MqttSettingStruct mqttSettings;
extern aquamqtt::WifiConfigStruct  wifiSettings;

namespace aquamqtt
{
WebServer WebHandler::serverWeb(80);

WebHandler::WebHandler()
{
}

const char HTTP_HEADER[] PROGMEM =
        "<head>"
        "<script type='text/javascript' src='web/js/jquery-min.js'></script>"
        "<script type='text/javascript' src='web/js/bootstrap.min.js'></script>"
        "<script type='text/javascript' src='web/js/functions.js'></script>"
        "<link href='web/css/bootstrap.min.css' rel='stylesheet' type='text/css' />"
        "<link href='web/css/style.css' rel='stylesheet' type='text/css' />"
        " </head>"
        "<body>"
        "<nav class='navbar navbar-expand-lg navbar-light bg-light rounded'><a class='navbar-brand' "
        "href='/'><strong>AquaMQTTConfig </strong> VERSION </a>"
        "<button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarNavDropdown' "
        "aria-controls='navbarNavDropdown' aria-expanded='false' aria-label='Toggle navigation'>"
        "<span class='navbar-toggler-icon'></span>"
        "</button>"
        "<div id='navbarNavDropdown' class='collapse navbar-collapse justify-content-md-center'>"
        "<ul class='navbar-nav'>"
        "<li class='nav-item'>"
        "<a class='nav-link' href='/'>WiFi</a>"
        "</li>"
        "<li class='nav-item'>"
        "<a class='nav-link' href='/MQTT'>MQTT</a>"
        "</li>"
        "<li class='nav-item'>"
        "<a class='nav-link' href='/tools'>Tools</a>"
        "</li>"
        "</ul></div>"
        "</nav>";

const char HTTP_WIFI[] PROGMEM =
        "<h1>Config WiFi</h1>"
        "<div class='row justify-content-md-center' >"
        "<div class='col-sm-6'><form method='POST' action='savewifi'>"
        "<div class='form-group mb-3'>"
        "<label for='ssid'>SSID</label>"
        "<input class='form-control' id='ssid' type='text' name='WIFISSID' value='{{ssid}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='pass'>Password</label>"
        "<input class='form-control' id='pass' type='password' name='WIFIpassword' value='{{wifipw}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='ip'>IP Adress</label>"
        "<input class='form-control' id='ip' type='text' name='ipAddress' value='{{ip}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='mask'>Mask Adress</label>"
        "<input class='form-control' id='mask' type='text' name='ipMask' value='{{mask}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='gateway'>Gateway Adress</label>"
        "<input type='text' class='form-control' id='gateway' name='ipGW' value='{{gw}}'>"
        "</div>"
        " <div class='form-group mb-3'>"
        "<button type='submit' class='btn btn-primary mb-3' name='save'>Save</button>"
        "</div>"
        "</form>";

const char HTTP_MQTT[] PROGMEM =
        "<h1>Config MQTT</h1>"
        "<div class='row justify-content-md-center'>"
        "<div class='col-sm-6'>"
        "<form method='POST' action='savemqtt'>"
        "<div class='form-group mb-3'>"
        "<label for='mqtt_server'>Server MQTT</label>"
        "<input class='form-control' id='mqtt_server' type='text' name='MQTT Server' value='{{mqtt_server}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='mqtt_port'>MQTT server Port</label>"
        "<input class='form-control' id='mqtt_port' type='numeric' name='MQTT Server Port' value='{{mqtt_port}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='mqtt_user'>UserName</label>"
        "<input class='form-control' id='mqtt_user' type='text' name='MQTT UserName' value='{{mqtt_user}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='mqtt_password'>Password</label>"
        "<input class='form-control' id='mqtt_password' type='password' name='MQTT Password' "
        "value='{{mqtt_password}}'>"
        "</div>"
        "<div class='form-group form-switch mb-3'>"
        "<input class='form-check-input' id='enableDiscovery' type='checkbox' name='enableDiscovery'"
        "{{discoveryenabled}}>"
        "<label class='form-check-label' for='enableDiscovery'>Enable HomeAssistant Discovery</label>"
        "</div>"
        " <div class='form-group mb-3'>"
        "<label for='haDiscoveryPrefix'>Discovery prefix</label>"
        "<input class='form-control' id='haDiscoveryPrefix' type='text' name='Mqtt prefix' "
        "value={{haDiscoveryPrefix}}>"
        "</div>"
        " <div class='form-group mb-3'>"
        "<button type='submit' class='btn btn-primary mb-3' name='save'>Save</button>"
        "</div>"
        "</form>";

const char HTTP_APIWEB[] PROGMEM =
        "<h1>API Web</h1>"
        "<div class='row justify-content-md-center' >"
        "<div class='col-sm-6'><form method='post' action='savepanel'>"
        "<div class='form-group'>"
        "<label for='text'>TEXT</label>"
        "<input class='form-control' id='text' type='text' name='text' value='{{text}}'>"
        "</div>"
        "<div class='form-group'>"
        "<label for='size'>Size</label>"
        "<select class='form-control' id='size' name='size'>"
        "<option value='1' {{selMin}}>Min</option>"
        "<option value='2' {{selMax}}>Max</option>"
        "</select>"

        "</div>"
        "<div class='form-group'>"
        "<label for='light'>Light</label>"
        "<input class='form-control' id='light' maxlength='4' inputmode='numeric' name='light' "
        "value='{{light}}'>"
        "</div>"
        "<div class='form-group'>"
        "<label for='scroll'>Scroll</label>"
        "<select class='form-control' id='scroll' name='scroll'>"
        "<option value='1' {{scrollOui}}>Oui</option>"
        "<option value='0' {{scrollNon}}>Non</option>"
        "</select>"

        "</div>"
        "<div class='form-group'>"
        "<label for='posx'>X</label>"
        "<input class='form-control' id='posx' type='text' inputmode='numeric' name='x' value='{{xText}}'>"
        "</div>"
        "<div class='form-group'>"
        "<label for='posy'>Y</label>"
        "<input type='text' class='form-control' id='posy' inputmode='numeric' name='y' value='{{yText}}'>"
        "</div>"
        "<button type='submit' class='btn btn-primary mb-2'name='save'>Refresh</button>"
        "</form>";

void WebHandler::setup()
{
    serverWeb.serveStatic("/web/js/jquery-min.js", LittleFS, "/web/js/jquery-min.js");
    serverWeb.serveStatic("/web/js/functions.js", LittleFS, "/web/js/functions.js");
    serverWeb.serveStatic("/web/js/bootstrap.min.js", LittleFS, "/web/js/bootstrap.min.js");
    serverWeb.serveStatic("/web/js/bootstrap.min.js.map", LittleFS, "/web/js/bootstrap.min.js.map");
    serverWeb.serveStatic("/web/css/bootstrap.min.css", LittleFS, "/web/css/bootstrap.min.css");
    serverWeb.serveStatic("/web/css/style.css", LittleFS, "/web/css/style.css");
    // serverWeb.serveStatic("/web/img/logo.png", LittleFS, "/web/img/logo.png");
    // serverWeb.serveStatic("/web/img/wait.gif", LittleFS, "/web/img/wait.gif");
    // serverWeb.serveStatic("/web/img/", LittleFS, "/web/img/");
    serverWeb.on("/", handleRoot);
    serverWeb.on("/savewifi", HTTP_POST, handleSaveWifiConfig);
    serverWeb.on("/savemqtt", HTTP_POST, handleSaveMQTTConfig);
    serverWeb.on("/saveothers", HTTP_POST, handleSaveOtherConfig);
    serverWeb.on("/savepanel", HTTP_POST, handleSavePanel);
    serverWeb.on("/api", HTTP_GET, handleAPI);
    serverWeb.on("/apiweb", handleAPIWeb);
    serverWeb.on("/tools", handleTools);
    serverWeb.on("/MQTT", handleMQTT);
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
    result += FPSTR(HTTP_HEADER);
    result += FPSTR(HTTP_MQTT);
    result += F("</html>");

    result.replace("{{version}}", "VERSION");
    result.replace("{{mqtt_server}}", mqttSettings.mqtt_server);
    result.replace("{{mqtt_port}}", mqttSettings.mqtt_port);
    result.replace("{{mqtt_user}}", mqttSettings.mqtt_user);
    result.replace("{{mqtt_password}}", mqttSettings.mqtt_password);
    bool enable = (mqttSettings.enableDiscovery.toInt() != 0);

    if (enable)
    {
        result.replace("{{discoveryenabled}}", "selected");
    }
    else
    {
        result.replace("{{discoveryenabled}}", "");
    }

    result.replace("{{haDiscoveryPrefix}}", mqttSettings.haDiscoveryPrefix);

    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::handleRoot()
{
    String result;
    result += F("<html>");
    result += FPSTR(HTTP_HEADER);
    result += FPSTR(HTTP_WIFI);
    result += F("</html>");
    result.replace("{{version}}", "VERSION");
    result.replace("{{ssid}}", String(wifiSettings.ssid));
    result.replace("{{wifipw}}", wifiSettings.password);
    // result.replace("{{ip}}", wifiSettings.ipAddress);
    // result.replace("{{mask}}", wifiSettings.ipMask);
    // result.replace("{{gw}}", wifiSettings.ipGW);

    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::handleSavePanel()
{
    // if (!serverWeb.hasArg("text"))
    // {
    //     serverWeb.send(500, "text/plain", "BAD ARGS");
    //     return;
    // }

    // String configPanel;
    // // text       = serverWeb.arg("text");
    // // sizeText   = serverWeb.arg("size").toInt();
    // // scrollText = serverWeb.arg("scroll").toInt();
    // // xText      = serverWeb.arg("x").toInt();
    // // yText      = serverWeb.arg("y").toInt();
    // // light      = serverWeb.arg("light").toInt();

    // configPanel = "{\"text\":\"" + text + "\",\"sizeText\":\"" + sizeText + "\",\"scrollText\":\"" + scrollText
    //               + "\",\"xText\":\"" + xText + "\",\"yText\":\"" + yText + "\",\"light\":\"" + light + "\"}";
    // Serial.println(configPanel);
    // StaticJsonDocument<512> jsonBuffer;
    // DynamicJsonDocument     doc(1024);
    // deserializeJson(doc, configPanel);

    // File panelFile = LittleFS.open("/config/panel.json", "w");
    // if (!panelFile)
    // {
    // }
    // else
    // {
    //     serializeJson(doc, panelFile);
    // }
    // serverWeb.sendHeader(F("Location"), F("/apiweb"));
    // serverWeb.send(303);
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
    }
    else
    {
        serializeJson(doc, configFile);
    }
    serverWeb.send(
            200,
            "text/html",
            "Save config OK ! <br><form method='GET' action='reboot'><input type='submit' name='reboot' "
            "value='Reboot'></form>");
}

void WebHandler::handleSaveMQTTConfig()
{
    // if (!serverWeb.hasArg("WIFISSID"))
    // {
    //     serverWeb.send(500, "text/plain", "BAD ARGS");
    //     return;
    // }
    JsonDocument doc;
    doc["mqtt_server"]       = serverWeb.arg("WIFISSID");
    doc["mqtt_port"]         = serverWeb.arg("WIFIpassword");
    doc["mqtt_user"]         = serverWeb.arg("ipAddress");
    doc["mqtt_password"]     = serverWeb.arg("ipMask");
    doc["enableDiscovery"]   = serverWeb.arg("ipGW");
    doc["haDiscoveryPrefix"] = serverWzeb.arg("");

    File configFile = LittleFS.open("/config/wifi.json", "w");
    if (!configFile)
    {
    }
    else
    {
        serializeJson(doc, configFile);
    }
    serverWeb.send(
            200,
            "text/html",
            "Save config OK ! <br><form method='GET' action='reboot'><input type='submit' name='reboot' "
            "value='Reboot'></form>");
}

void WebHandler::handleTools()
{
    String result;

    result += F("<html>");
    result += FPSTR(HTTP_HEADER);
    result += F("<h1>Tools</h1>");
    result += F("<div class='btn-group-vertical'>");
    result += F("<a href='/fsbrowser' class='btn btn-primary mb-2'>FSbrowser</button>");
    result += F("<a href='/update' class='btn btn-primary mb-2'>Update</button>");
    result += F("<a href='/reboot' class='btn btn-primary mb-2'>Reboot</button>");
    result += F("</div></body></html>");

    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::handleReboot()
{
    String result;

    result += F("<html>");
    result += FPSTR(HTTP_HEADER);
    result += F("<h1>Reboot ...</h1>");
    result = result + F("</body></html>");
    serverWeb.sendHeader(F("Location"), F("/"));
    serverWeb.send(303);

    ESP.restart();
}

void WebHandler::handleUpdate()
{
    String result;

    result += F("<html>");
    result += FPSTR(HTTP_HEADER);
    result += F("<h1>Update ...</h1>");
    result += F("<div class='row justify-content-md-center' >");
    result += F("<div class='col-sm-6'><form method='POST' action='StartOTA' enctype='multipart/form-data'>");
    result += F("<div class='form-group'>");
    result += F(" <label for='ip'>File : </label>");
    result += F(" <input type='file' name='file' class='form-control' id='file' />");
    result += F("</div>");
    result += F("<button type='submit' class='btn btn-primary mb-2'>Update</button>");
    result = result + F("</form></body></html>");

    serverWeb.send(200, F("text/html"), result);
}

void WebHandler::handleFSbrowser()
{
    String result;
    result += F("<html>");
    result += FPSTR(HTTP_HEADER);
    result += F("<h1>FSBrowser</h1>");
    result += F("<nav id='navbar-custom' class='navbar navbar-default navbar-fixed-left'>");
    result += F("      <div class='navbar-header'>");
    result += F("        <!--<a class='navbar-brand' href='#'>Brand</a>-->");
    result += F("      </div>");
    result += F("<ul class='nav navbar-nav'>");

    String str  = "";
    File   dir  = LittleFS.open("/config/");
    File   file = dir.openNextFile();
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
        file = file.openNextFile();
    }
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
    result += F("<button type='submit' class='btn btn-primary mb-2'>Save</button>");
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