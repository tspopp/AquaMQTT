#include "config/config.h"

#include <ArduinoJson.h>
#include <WiFi.h>

#include "Arduino.h"

aquamqtt::WifiConfigStruct  wifiSettings;
aquamqtt::MqttSettingStruct mqttSettings;
aquamqtt::AquaMqttStruct    aquamqttSettings;

IPAddress aquamqtt::parse_ip_address(const char* str)
{
    IPAddress result;
    int       index = 0;

    result[0] = 0;
    while (*str)
    {
        if (isdigit((unsigned char) *str))
        {
            result[index] *= 10;
            result[index] += *str - '0';
        }
        else
        {
            index++;
            if (index < 4)
            {
                result[index] = 0;
            }
        }
        str++;
    }

    return result;
}

bool aquamqtt::loadWifiConfig()
{
    File wifiConfig = LittleFS.open("/config/wifi.json", "r+");
    if (!wifiConfig)
    {
        Serial.println("[error] Can't open Wifi Configuration file");
        return false;
    }
    size_t size = wifiConfig.size();
    if (size > 1024)
    {
        Serial.println("[error] Wifi file size too big");
        return false;
    }

    JsonDocument doc;
    deserializeJson(doc, wifiConfig);
    if (doc.isNull())
    {
        Serial.println("[error] Can't deserialize json doc for wifi");
        return false;
    }
    if (strlen(doc["ssid"]) == 0)
    {
        Serial.println("[error] SSID must not be empty");
        return false;
    }
    wifiSettings.ssid     = doc["ssid"].as<String>();
    wifiSettings.password = doc["password"].as<String>();
    wifiConfig.close();
    return true;
}

bool aquamqtt::loadConfig()
{
    File configFile = LittleFS.open("/config/config.json", "r+");
    if (!configFile)
    {
        return false;
    }

    size_t size = configFile.size();
    if (size > 1024)
    {
        return false;
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);

    // StaticJsonDocument<512> jsonBuffer;
    JsonDocument doc;
    deserializeJson(doc, buf.get());

    if (doc.isNull())
    {
        return false;
    }
    const char* ssid = doc["ssid"];
    // double tmp = doc["tmp"];
    // ConfigSettings.ssid = ssid;
    // char pass_[30];
    // // strcpy(pass_, doc["pass"]);
    // ConfigSettings.password = doc["pass"];
    // // char ip_[30];
    // // strcpy(ip_, doc["pass");
    // ConfigSettings.ipAddress = doc["ip"];
    // // char mask_[30];
    // // strcpy(mask_, doc["mask"]);
    // ConfigSettings.ipMask = doc["mask"];
    // // char gw_[30];
    // // strcpy(gw_, doc["gw"]);
    // ConfigSettings.ipGW = doc["gw"];

    configFile.close();

    return true;
}

bool aquamqtt::loadMqttConfig()
{
    File mqttConfig = LittleFS.open("/config/mqtt.json", "r+");
    if (!mqttConfig)
    {
        Serial.println("[error] Can't open Mqtt Configuration file");
        return false;
    }
    size_t size = mqttConfig.size();
    if (size > 1024)
    {
        Serial.println("[error] mqtt file size too big");
        return false;
    }

    JsonDocument doc;
    deserializeJson(doc, mqttConfig);
    if (doc.isNull())
    {
        Serial.println("[error] Can't deserialize json doc for mqtt");
        return false;
    }

    mqttSettings.mqtt_server       = doc["mqtt_server"].as<String>();
    mqttSettings.mqtt_port         = doc["mqtt_port"].as<int>();
    mqttSettings.mqtt_user         = doc["mqtt_user"].as<String>();
    mqttSettings.mqtt_password     = doc["mqtt_password"].as<String>();
    mqttSettings.enableDiscovery   = doc["enableDiscovery"].as<bool>();
    mqttSettings.haDiscoveryPrefix = doc["haDiscoveryPrefix"].as<String>();
    mqttSettings.brokerClientId    = doc["brokerClientId"].as<String>();  
    mqttConfig.close();

    return true;
}

bool aquamqtt::loadAquaMqttConfig()
{
    File aquamqttConfig = LittleFS.open("/config/aquamqtt.json", "r+");
    if (!aquamqttConfig)
    {
        Serial.println("[error] Can't open AquaMqtt Configuration file");
        return false;
    }
    size_t size = aquamqttConfig.size();
    if (size > 1024)
    {
        Serial.println("[error] aquamqtt file size too big");
        return false;
    }

    JsonDocument doc;
    deserializeJson(doc, aquamqttConfig);
    if (doc.isNull())
    {
        Serial.println("[error] Can't deserialize json doc for aquamqtt");
        return false;
    }

    aquamqttSettings.heatpumpModelName = doc["heatpumpModelName"].as<String>();
    aquamqttSettings.operationMode     = doc["operationMode"].as<int>();

    aquamqttConfig.close();

    return true;
}