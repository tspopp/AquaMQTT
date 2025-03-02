#ifndef AQUAMQTT_CONFIG2_H
#define AQUAMQTT_CONFIG2_H
#include <Arduino.h>

#include "LittleFS.h"

namespace aquamqtt
{
struct WifiConfigStruct
{
    String ssid;
    String password;
    String ipAddress;
    String ipMask;
    String ipGW;
};

struct MqttSettingStruct
{
    String mqtt_server;
    String mqtt_port;
    String mqtt_user;
    String mqtt_password;
    String enableDiscovery;
    String haDiscoveryPrefix;
};

IPAddress parse_ip_address(const char* str);
bool      loadConfig();
bool      loadWifiConfig();
bool      loadMqttConfig();
}  // namespace aquamqtt
#endif  // AQUAMQTT_CONFIG2_H