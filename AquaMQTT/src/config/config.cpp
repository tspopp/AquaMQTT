#include "config.h"

#include <ArduinoJson.h>
#include <WiFi.h>

#include "Arduino.h"

aquamqtt::WifiConfigStruct     wifiSettings;
aquamqtt::ConfigSettingsStruct configSettings;

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
    Serial.println(wifiConfig);
    if (doc.isNull())
    {
        Serial.println("[error] Can't deserialize json doc for wifi");
        return false;
    }

    wifiSettings.ssid     = doc["ssid"]; //Compile error
    wifiSettings.password = doc["password"];

    Serial.println("Read SSID is");
    Serial.println(wifiSettings.ssid);
    Serial.println("Read PWD is");
    Serial.println(wifiSettings.password);

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

//   File panelFile = SPIFFS.open("/config/config.json", "r+");
//   if (!panelFile) {
//     return false;
//   }

//   size_t size = panelFile.size();
//   if (size > 1024) {
//     return false;
//   }

//   // Allocate a buffer to store contents of the file.
//   std::unique_ptr<char[]> buf(new char[size]);

//   panelFile.readBytes(buf.get(), size);

//   DynamicJsonDocument doc(1024);
//   deserializeJson(doc, buf.get());

//   if (doc.isNull()) {
//     return false;
//   }

//   char txt_[512];
//   strcpy(txt_, doc["text"]);
//   cfgPanel.text = String(txt_);
//   char sz_[2];
//   strcpy(sz_, doc["sizeText"]);
//   cfgPanel.sizeText = String(sz_);
//   char scroll_[2];
//   strcpy(scroll_, doc["scrollText"]);
//   cfgPanel.scrollText = String(scroll_);
//   char x_[2];
//   strcpy(x_, doc["xText"]);
//   cfgPanel.xText = String(x_);
//   char y_[2];
//   strcpy(y_, doc["yText"]);
//   cfgPanel.yText = String(y_);
//   char light_[6];
//   strcpy(light_, doc["light"]);
//   cfgPanel.light = String(light_);

//     panelFile.close();

//     return true;
// }