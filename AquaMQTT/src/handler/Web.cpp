#include "handler/Web.h"

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <LittleFS.h>

#include "config/WebConfig.h"
#include "task/ModbusRelayTask.h"

extern aquamqtt::ModbusRelayTask modbusRelayTask;

namespace aquamqtt
{

AsyncWebServer WebHandler::mServer(80);

WebHandler::WebHandler()
{
}

void WebHandler::setup()
{
    // Serve static files
    mServer.serveStatic("/web/", LittleFS, "/web/");

    // Root page
    mServer.on("/", HTTP_GET, handleRoot);

    // GET API endpoints — return stored config JSON
    mServer.on("/api/wifi", HTTP_GET, handleWifiGet);
    mServer.on("/api/mqtt", HTTP_GET, handleMqttGet);
    mServer.on("/api/aquamqtt", HTTP_GET, handleAquaGet);

    // Reboot endpoint
    mServer.on("/reboot", HTTP_GET, handleReboot);

    // Diagnostics endpoint
    mServer.on("/api/diag", HTTP_GET, [](AsyncWebServerRequest* request) {
        String json = "{\"framesReceived\":";
        json += modbusRelayTask.getFramesReceived();
        json += ",\"crcErrors\":";
        json += modbusRelayTask.getCrcErrors();
        json += ",\"framesRelayed\":";
        json += modbusRelayTask.getFramesRelayed();
        json += ",\"hmiFramesIn\":";
        json += modbusRelayTask.getHmiFramesIn();
        json += ",\"mainFramesIn\":";
        json += modbusRelayTask.getMainFramesIn();
        json += ",\"hmiBytesIn\":";
        json += modbusRelayTask.getHmiBytesIn();
        json += ",\"mainBytesIn\":";
        json += modbusRelayTask.getMainBytesIn();
        json += ",\"echoBytes\":";
        json += modbusRelayTask.getEchoBytes();
        json += ",\"txBytesWritten\":";
        json += modbusRelayTask.getTxBytesWritten();
        json += ",\"writesInjected\":";
        json += modbusRelayTask.getWritesInjected();
        json += ",\"lastFrame\":\"";
        for (int i = 0; i < modbusRelayTask.getLastFwdFrameLen(); i++) {
            char hex[4];
            snprintf(hex, sizeof(hex), "%02X ", modbusRelayTask.getLastFwdFrame()[i]);
            json += hex;
        }
        json += "\"";
        json += ",\"uptime\":";
        json += millis();
        json += "}";

        request->send(200, "application/json", json);
    });

    // POST API: WiFi config
    auto* wifiHandler = new AsyncCallbackJsonWebHandler("/api/wifi");
    wifiHandler->setMethod(HTTP_POST);
    wifiHandler->onRequest([](AsyncWebServerRequest* request, JsonVariant& json) {
        JsonObject root = json.as<JsonObject>();
        bool reboot = root["reboot"] | false;
        JsonObject data = root["data"].as<JsonObject>();

        String ssid = data["ssid"] | "";
        if (ssid.length() == 0)
        {
            request->send(400, "application/json", "{\"error\":\"SSID required\"}");
            return;
        }

        String output;
        serializeJson(data, output);
        if (saveConfigFile("wifi", output))
        {
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        }
        else
        {
            request->send(500, "application/json", "{\"error\":\"write failed\"}");
            return;
        }

        if (reboot)
        {
            delay(1000);
            ESP.restart();
        }
    });

    // POST API: MQTT config
    auto* mqttHandler = new AsyncCallbackJsonWebHandler("/api/mqtt");
    mqttHandler->setMethod(HTTP_POST);
    mqttHandler->onRequest([](AsyncWebServerRequest* request, JsonVariant& json) {
        JsonObject root = json.as<JsonObject>();
        bool reboot = root["reboot"] | false;
        JsonObject data = root["data"].as<JsonObject>();

        String output;
        serializeJson(data, output);
        if (saveConfigFile("mqtt", output))
        {
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        }
        else
        {
            request->send(500, "application/json", "{\"error\":\"write failed\"}");
            return;
        }

        if (reboot)
        {
            delay(1000);
            ESP.restart();
        }
    });

    // POST API: AquaMQTT config
    auto* aquaHandler = new AsyncCallbackJsonWebHandler("/api/aquamqtt");
    aquaHandler->setMethod(HTTP_POST);
    aquaHandler->onRequest([](AsyncWebServerRequest* request, JsonVariant& json) {
        JsonObject root = json.as<JsonObject>();
        bool reboot = root["reboot"] | false;
        JsonObject data = root["data"].as<JsonObject>();

        String output;
        serializeJson(data, output);
        if (saveConfigFile("aquamqtt", output))
        {
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        }
        else
        {
            request->send(500, "application/json", "{\"error\":\"write failed\"}");
            return;
        }

        if (reboot)
        {
            delay(1000);
            ESP.restart();
        }
    });

    mServer.addHandler(wifiHandler);
    mServer.addHandler(mqttHandler);
    mServer.addHandler(aquaHandler);

    mServer.onNotFound(handleNotFound);
    mServer.begin();
    Serial.println("[web] server started on port 80");
}

void WebHandler::loop()
{
}

bool WebHandler::saveConfigFile(const String& filename, const String& content)
{
    String path = "/config/" + filename + ".json";
    File file = LittleFS.open(path, "w");
    if (!file)
    {
        Serial.printf("[web] failed to open %s for writing\n", path.c_str());
        return false;
    }
    file.print(content);
    file.close();
    return true;
}

void WebHandler::handleRoot(AsyncWebServerRequest* request)
{
    if (LittleFS.exists("/index.html"))
    {
        request->send(LittleFS, "/index.html", "text/html");
    }
    else
    {
        // Serve a minimal embedded page if no filesystem HTML available
        request->send(200, "text/html",
            "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
            "<title>AquaMQTT Config</title><style>body{font-family:sans-serif;background:#1a1a2e;color:#e0e0e0;padding:20px;max-width:600px;margin:0 auto}"
            "h1{color:#4fc3f7}label{display:block;margin:12px 0 4px;color:#90caf9;font-size:.85em}"
            "input,select{width:100%;padding:8px;border:1px solid #333;border-radius:4px;background:#0f3460;color:#e0e0e0;box-sizing:border-box}"
            ".tabs{display:flex;gap:4px;margin:20px 0}.tab{padding:8px 16px;background:#16213e;border:1px solid #333;border-radius:4px;cursor:pointer;color:#aaa}"
            ".tab.active{background:#0f3460;color:#4fc3f7}.panel{display:none}.panel.active{display:block}"
            ".btn{padding:10px 20px;border:none;border-radius:4px;cursor:pointer;font-weight:600;margin:4px}"
            ".btn-save{background:#4fc3f7;color:#000}.btn-reboot{background:#ff7043;color:#fff}"
            "#alert{padding:10px;border-radius:4px;margin:10px 0;display:none}</style></head><body>"
            "<h1>AquaMQTT Config</h1><p style='color:#888'>Optitronic 2</p><div id='alert'></div>"
            "<div class='tabs'><div class='tab active' data-tab='wifi'>WiFi</div><div class='tab' data-tab='mqtt'>MQTT</div><div class='tab' data-tab='aquamqtt'>Device</div></div>"
            "<div class='panel active' id='panel-wifi'><form id='form-wifi'>"
            "<label>SSID</label><input id='ssid' name='ssid'>"
            "<label>Password</label><input id='password' name='password' type='password'>"
            "<label>Network Name</label><input id='networkName' name='networkName'>"
            "</form></div>"
            "<div class='panel' id='panel-mqtt'><form id='form-mqtt'>"
            "<label>Server</label><input id='server' name='server'>"
            "<label>Port</label><input id='port' name='port' type='number'>"
            "<label>User</label><input id='user' name='user'>"
            "<label>Password</label><input id='mqtt_password' name='password' type='password'>"
            "<label>Client ID</label><input id='clientId' name='clientId'>"
            "<label><input id='enableDiscovery' name='enableDiscovery' type='checkbox'> HA Discovery</label>"
            "<label>Discovery Prefix</label><input id='discoveryPrefix' name='discoveryPrefix'>"
            "</form></div>"
            "<div class='panel' id='panel-aquamqtt'><form id='form-aquamqtt'>"
            "<label>Model Name</label><input id='heatpumpModelName' name='heatpumpModelName'>"
            "<label>Mode</label><select id='operationMode' name='operationMode'>"
            "<option value='0'>LISTENER</option><option value='1'>MITM</option>"
            "<option value='2'>OPTITRONIC2_LISTENER</option><option value='3'>OPTITRONIC2_MITM</option></select>"
            "</form></div>"
            "<div><button class='btn btn-save' id='btn-save'>Save</button>"
            "<button class='btn btn-reboot' id='btn-save-reboot'>Save &amp; Reboot</button></div>"
            "<script>"
            "document.querySelectorAll('.tab').forEach(t=>t.onclick=()=>{"
            "document.querySelectorAll('.tab').forEach(x=>x.classList.remove('active'));"
            "document.querySelectorAll('.panel').forEach(x=>x.classList.remove('active'));"
            "t.classList.add('active');document.getElementById('panel-'+t.dataset.tab).classList.add('active')});"
            "async function load(){try{let w=await(await fetch('/api/wifi')).json();"
            "if(w.ssid)document.getElementById('ssid').value=w.ssid;"
            "if(w.password)document.getElementById('password').value=w.password;"
            "if(w.networkName)document.getElementById('networkName').value=w.networkName}catch(e){}"
            "try{let m=await(await fetch('/api/mqtt')).json();"
            "if(m.server)document.getElementById('server').value=m.server;"
            "if(m.port)document.getElementById('port').value=m.port;"
            "if(m.user)document.getElementById('user').value=m.user;"
            "if(m.password)document.getElementById('mqtt_password').value=m.password;"
            "if(m.clientId)document.getElementById('clientId').value=m.clientId;"
            "document.getElementById('enableDiscovery').checked=m.enableDiscovery||false;"
            "if(m.discoveryPrefix)document.getElementById('discoveryPrefix').value=m.discoveryPrefix}catch(e){}"
            "try{let a=await(await fetch('/api/aquamqtt')).json();"
            "if(a.heatpumpModelName)document.getElementById('heatpumpModelName').value=a.heatpumpModelName;"
            "if(a.operationMode!==undefined)document.getElementById('operationMode').value=a.operationMode}catch(e){}}"
            "function getEP(){let a=document.querySelector('.tab.active');return a?a.dataset.tab:'wifi'}"
            "function collect(ep){let f=document.getElementById('form-'+ep),d={};f.querySelectorAll('input,select').forEach(e=>{"
            "let k=e.name||e.id;if(e.type==='checkbox')d[k]=e.checked;else if(e.type==='number'||e.tagName==='SELECT')d[k]=parseInt(e.value)||0;else d[k]=e.value});return d}"
            "async function save(r){let ep=getEP(),d=collect(ep);try{let res=await fetch('/api/'+ep,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({reboot:r,data:d})});"
            "let al=document.getElementById('alert');if(res.ok){al.style.display='block';al.style.background='#1b5e20';al.textContent='Saved'+(r?' - rebooting...':'')}"
            "else{al.style.display='block';al.style.background='#b71c1c';al.textContent='Error'}}catch(e){"
            "let al=document.getElementById('alert');al.style.display='block';al.style.background=r?'#1b5e20':'#b71c1c';al.textContent=r?'Rebooting...':'Connection error'}}"
            "document.getElementById('btn-save').onclick=()=>save(false);"
            "document.getElementById('btn-save-reboot').onclick=()=>save(true);load();"
            "</script></body></html>");
    }
}

void WebHandler::handleNotFound(AsyncWebServerRequest* request)
{
    request->send(404, "text/plain", "Not Found");
}

void WebHandler::handleWifiGet(AsyncWebServerRequest* request)
{
    request->send(LittleFS, "/config/wifi.json", "application/json");
}

void WebHandler::handleMqttGet(AsyncWebServerRequest* request)
{
    request->send(LittleFS, "/config/mqtt.json", "application/json");
}

void WebHandler::handleAquaGet(AsyncWebServerRequest* request)
{
    request->send(LittleFS, "/config/aquamqtt.json", "application/json");
}

void WebHandler::handleReboot(AsyncWebServerRequest* request)
{
    request->send(200, "text/plain", "Rebooting...");
    delay(1000);
    ESP.restart();
}

}  // namespace aquamqtt
