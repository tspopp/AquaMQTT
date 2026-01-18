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
    String networkName;
};

struct MqttSettingStruct
{
    String mqtt_server;
    int    mqtt_port;
    String mqtt_user;
    String mqtt_password;
    bool   enableDiscovery;
    String haDiscoveryPrefix;
    String brokerClientId;
};

struct AquaMqttStruct
{
    String heatpumpModelName;
    int    operationMode;
};

bool      loadConfig();
bool      loadWifiConfig();
bool      loadMqttConfig();
bool      loadAquaMqttConfig();
}  // namespace aquamqtt
#endif  // AQUAMQTT_CONFIG2_H