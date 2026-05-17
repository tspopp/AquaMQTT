#include "config/WebConfig.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

#include "config/Configuration.h"

namespace aquamqtt
{

WifiConfig wifiConfig;
MqttConfig mqttConfig;
AquaMqttConfig aquaMqttConfig;

static void ensureDir(const char* path)
{
    if (!LittleFS.exists(path))
    {
        LittleFS.mkdir(path);
    }
}

static void createDefaultWifi()
{
    ensureDir("/config");
    File f = LittleFS.open("/config/wifi.json", "w");
    if (f)
    {
        f.print("{\"ssid\":\"");
        f.print(config::ssid);
        f.print("\",\"password\":\"");
        f.print(config::psk);
        f.print("\",\"networkName\":\"aquamqtt\"}");
        f.close();
    }
}

static void createDefaultMqtt()
{
    ensureDir("/config");
    File f = LittleFS.open("/config/mqtt.json", "w");
    if (f)
    {
        JsonDocument doc;
        doc["server"] = config::brokerAddr;
        doc["port"] = config::brokerPort;
        doc["user"] = config::brokerUser;
        doc["password"] = config::brokerPassword;
        doc["clientId"] = config::brokerClientId;
        doc["enableDiscovery"] = true;
        doc["discoveryPrefix"] = "homeassistant/";
        serializeJson(doc, f);
        f.close();
    }
}

static void createDefaultAquaMqtt()
{
    ensureDir("/config");
    File f = LittleFS.open("/config/aquamqtt.json", "w");
    if (f)
    {
        JsonDocument doc;
        doc["heatpumpModelName"] = config::heatpumpModelName;
        doc["operationMode"] = static_cast<int>(config::OPERATION_MODE);
        serializeJson(doc, f);
        f.close();
    }
}

bool loadWifiConfig()
{
    File file = LittleFS.open("/config/wifi.json", "r");
    if (!file)
    {
        Serial.println("[config] wifi.json not found, creating defaults");
        createDefaultWifi();
        file = LittleFS.open("/config/wifi.json", "r");
        if (!file) return false;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        Serial.print("[config] wifi.json parse error: ");
        Serial.println(err.c_str());
        return false;
    }

    wifiConfig.ssid = doc["ssid"] | "";
    wifiConfig.password = doc["password"] | "";
    wifiConfig.networkName = doc["networkName"] | "aquamqtt";

    if (wifiConfig.ssid.length() == 0)
    {
        Serial.println("[config] wifi SSID is empty");
        return false;
    }

    return true;
}

bool loadMqttConfig()
{
    File file = LittleFS.open("/config/mqtt.json", "r");
    if (!file)
    {
        Serial.println("[config] mqtt.json not found, creating defaults");
        createDefaultMqtt();
        file = LittleFS.open("/config/mqtt.json", "r");
        if (!file) return false;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        Serial.print("[config] mqtt.json parse error: ");
        Serial.println(err.c_str());
        return false;
    }

    mqttConfig.server = doc["server"] | "localhost";
    mqttConfig.port = doc["port"] | 1883;
    mqttConfig.user = doc["user"] | "";
    mqttConfig.password = doc["password"] | "";
    mqttConfig.clientId = doc["clientId"] | "aquamqtt";
    mqttConfig.enableDiscovery = doc["enableDiscovery"] | true;
    mqttConfig.discoveryPrefix = doc["discoveryPrefix"] | "homeassistant/";

    return true;
}

bool loadAquaMqttConfig()
{
    File file = LittleFS.open("/config/aquamqtt.json", "r");
    if (!file)
    {
        Serial.println("[config] aquamqtt.json not found, creating defaults");
        createDefaultAquaMqtt();
        file = LittleFS.open("/config/aquamqtt.json", "r");
        if (!file) return false;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        Serial.print("[config] aquamqtt.json parse error: ");
        Serial.println(err.c_str());
        return false;
    }

    aquaMqttConfig.heatpumpModelName = doc["heatpumpModelName"] | "Austria Email WPA 450 ECO";
    aquaMqttConfig.operationMode = doc["operationMode"] | 3;  // OPTITRONIC2_MITM

    return true;
}

bool loadAllConfig()
{
    bool ok = true;
    if (!loadWifiConfig()) ok = false;
    if (!loadMqttConfig()) ok = false;
    if (!loadAquaMqttConfig()) ok = false;
    return ok;
}

}  // namespace aquamqtt
