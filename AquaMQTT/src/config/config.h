#ifndef AQUAMQTT_CONFIG2_H
#define AQUAMQTT_CONFIG2_H
#include <Arduino.h>
#include "LittleFS.h"

namespace aquamqtt
{
struct WifiConfigStruct
{
    const char* ssid;
    const char* password;
    char* ipAddress;
    char* ipMask;
    char* ipGW;
};

struct ConfigSettingsStruct
{
    char* mqtt_server;
    char* mqtt_port;
    char* mqtt_user;
    char* mqtt_password;
    bool  enableDiscovery;
    char* haDiscoveryPrefix;
};



IPAddress parse_ip_address(const char* str);
bool      loadConfig();
bool      loadWifiConfig();
}  // namespace aquamqtt
#endif  // AQUAMQTT_CONFIG2_H