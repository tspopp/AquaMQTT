#ifndef AQUAMQTT_WEBCONFIG_H
#define AQUAMQTT_WEBCONFIG_H

#include <Arduino.h>

namespace aquamqtt
{

struct WifiConfig
{
    String ssid;
    String password;
    String networkName;
};

struct MqttConfig
{
    String server;
    uint16_t port;
    String user;
    String password;
    String clientId;
    bool enableDiscovery;
    String discoveryPrefix;
};

struct AquaMqttConfig
{
    String heatpumpModelName;
    int operationMode;
};

// Global config instances (defined in WebConfig.cpp)
extern WifiConfig wifiConfig;
extern MqttConfig mqttConfig;
extern AquaMqttConfig aquaMqttConfig;

bool loadAllConfig();
bool loadWifiConfig();
bool loadMqttConfig();
bool loadAquaMqttConfig();

}  // namespace aquamqtt

#endif  // AQUAMQTT_WEBCONFIG_H
