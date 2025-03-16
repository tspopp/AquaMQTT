#ifndef AQUAMQTT_WEB_H
#define AQUAMQTT_WEB_H
#include <WebServer.h>
#include <WiFi.h>

#include "config/config.h"

namespace aquamqtt
{
class WebHandler
{
public:
    WebHandler();
    void setup();
    void loop();

private:
    static void handleRoot();
    static void handleNotFound();
    static void handleMQTT();
    static void handleSaveWifiConfig();
    static void handleDebug();
    static void handleReboot();
    static void reboot(bool perform, String newLocation);
    static void handleFSbrowser();
    static void handleReadfile();
    static void handleSaveMQTTConfig();
    static void handleAquaMQTT();
    static void handleSaveAquaMQTTConfig();

    static WebServer serverWeb;
};

namespace web
{

const char HTTP_HEADER[] PROGMEM =
        "<head>"
        "<script type='text/javascript' src='web/js/jquery-min.js'></script>"
        "<script type='text/javascript' src='web/js/bootstrap.min.js'></script>"
        "<script type='text/javascript' src='web/js/functions.js'></script>"
        "<link href='web/css/bootstrap.min.css' rel='stylesheet' type='text/css' />"
        "<link href='web/css/style.css' rel='stylesheet' type='text/css' />"
        "<!-- Modal -->"
        "<div class='modal fade' id='staticBackdrop' data-bs-backdrop='static' data-bs-keyboard='false' tabindex='-1'"
        " aria-labelledby='staticBackdropLabel' aria-hidden='true'>"
        "<div class='modal-dialog'>"
        "<div class='modal-content'>"
        "<div class='modal-header'>"
        "<h5 class='modal-title' id='staticBackdropLabel'>Confirmation</h5>"
        "<button type='button' class='btn-close' data-bs-dismiss='modal' aria-label='Close'></button>"
        "</div>"
        "<div class='modal-body'>"
        " Device reboot is necessary to take into account new settings."
        "Do you want to reboot after modification save ?"
        "</div>"
        "<div class='modal-footer'>"
        "<button type='button' id='save' class='btn btn-primary'>Save only</button>"
        "<button type='button' id='savereboot' class='btn btn-warning'>Save & reboot</button>"
        "<button type='button' class='btn btn-secondary' data-bs-dismiss='modal'>Cancel</button>"
        "</div>"
        "</div>"
        "</div>"
        "</div>"
        " </head>"
        "<body>"
        "<nav class='navbar navbar-expand-lg navbar-light bg-light rounded'><a class='navbar-brand' "
        "href='/'><img src='web/img/logo.png'/><strong>AquaMQTTConfig </strong> {{VERSION}} </a>"
        "<button class='navbar-toggler' type='button' data-bs-toggle='collapse' data-bs-target='#navbarNavDropdown' "
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
        "<a class='nav-link' href='/Aquamqtt'>AquaMqtt</a>"
        "</li>"
        "<li class='nav-item'>"
        "<a class='nav-link' href='/debug'>Debug</a>"
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
        "<input type='hidden' id='reboot' name='reboot' value='0'>"
        "</form>"
        " <div class='form-group mb-3'>"
        "<button type='submit' class='btn btn-primary mb-3' data-bs-toggle='modal' data-bs-target='#staticBackdrop' "
        "name='save'>Save</button>"
        "</div>";

const char HTTP_MQTT[] PROGMEM =
        "<h1>Config MQTT</h1>"
        "<div class='row justify-content-md-center'>"
        "<div class='col-sm-6'>"
        "<form method='POST' action='savemqtt'>"
        "<div class='form-group mb-3'>"
        "<label for='mqtt_server'>Server MQTT</label>"
        "<input class='form-control' id='mqtt_server' type='text' name='MQTT_Server' value='{{mqtt_server}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='mqtt_port'>MQTT server Port</label>"
        "<input class='form-control' id='mqtt_port' type='numeric' name='MQTT_ServerPort' value='{{mqtt_port}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='mqtt_user'>UserName</label>"
        "<input class='form-control' id='mqtt_user' type='text' name='MQTT_UserName' value='{{mqtt_user}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='mqtt_password'>Password</label>"
        "<input class='form-control' id='mqtt_password' type='password' name='' "
        "value='{{mqtt_password}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='brokerClientId'>ClientID</label>"
        "<input class='form-control' id='brokerClientId' type='text' name='MQTT_BrokerID' "
        "value={{brokerClientId}}>"
        "</div>"
        "<div class='form-group form-switch mb-3'>"
        "<input class='form-check-input' id='enableDiscovery' type='checkbox' name='enableDiscovery'"
        "{{discoveryenabled}}>"
        "<label class='form-check-label' for='enableDiscovery'>Enable HomeAssistant Discovery</label>"
        "</div>"
        " <div class='form-group mb-3'>"
        "<label for='haDiscoveryPrefix'>Discovery prefix</label>"
        "<input class='form-control' id='haDiscoveryPrefix' type='text' name='MQTT_Prefix' "
        "value={{haDiscoveryPrefix}}>"
        "</div>"
        "<input type='hidden' id='reboot' name='reboot' value='0'>"
        "</form>"
        " <div class='form-group mb-3'>"
        "<button type='submit' class='btn btn-primary mb-3' data-bs-toggle='modal' data-bs-target='#staticBackdrop' "
        "name='save'>Save</button>"
        "</div>";

const char HTTP_AQUA[] PROGMEM =
        "<h1>Config AquaMQTT</h1>"
        "<div class='row justify-content-md-center'>"
        "<div class='col-sm-6'>"
        "<form method='POST' action='saveaquamqtt'>"
        "<div class='form-group mb-3'>"
        "<label for='heatpump'>Heat Pump model name</label>"
        "<input class='form-control' id='heatpump' type='text' name='AQUA_HeatPump' value='{{aqua_heatpump}}'>"
        "</div>"
        "<div class='form-group mb-3'>"
        "<label for='operationMode'>operationMode</label>"
        "<select id='operationMode' name='AQUA_Mode' class=form-select form-select-md mb-3>"
        "<option value='0' {{SelLIST}}>LISTENER</option>"
        "<option value='1' {{SelMITM}}>MITM</option>"
        "</select>"
        "</div>"
        "<input type='hidden' id='reboot' name='reboot' value='0'>"
        "</form>"
        " <div class='form-group mb-3'>"
        "<button type='submit' class='btn btn-primary mb-3' data-bs-toggle='modal' data-bs-target='#staticBackdrop' "
        "name='save'>Save</button>"
        "</div>";

}  // namespace web
}  // namespace aquamqtt
#endif  // AQUAMQTT_WEB_H