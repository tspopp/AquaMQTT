#ifndef AQUAMQTT_EXAMPLECONFIGURATION_H
#define AQUAMQTT_EXAMPLECONFIGURATION_H

#include "Arduino.h"

namespace aquamqtt
{
namespace config
{

constexpr char     ssid[]           = "ExampleSSID";
constexpr char     psk[]            = "ExamplePSK";
constexpr char     brokerAddr[]     = "192.168.188.1";
constexpr uint16_t brokerPort       = 1883;
constexpr char     brokerClientId[] = "aquamqtt";
// leave blank if your broker does not require a username/password
constexpr char brokerUser[]     = "";
constexpr char brokerPassword[] = "";
constexpr char mqttPrefix[]     = "";

}  // namespace config
}  // namespace aquamqtt

#endif  // AQUAMQTT_EXAMPLECONFIGURATION_H
