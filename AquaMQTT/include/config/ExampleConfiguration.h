#ifndef AQUAMQTT_EXAMPLECONFIGURATION_H
#define AQUAMQTT_EXAMPLECONFIGURATION_H

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
constexpr char brokerUser[]        = "";
constexpr char brokerPassword[]    = "";
constexpr char mqttPrefix[]        = "";
constexpr char heatpumpModelName[] = "Atlantic Explorer v4";

}  // namespace config

namespace mqtt
{
// Optional and additional MQTT topics which are configured by the user.

// Possibility to publish the current heat pump power to an additional mqtt topic of your choice.
// Leave blank if not needed / unused
constexpr char optionalPublishTopicHeatPumpCurrentPower[] = { /*"openWB/LegacySmartHome/set/Devices/1/Aktpower"*/ };

// Possibility to publish the current heat element power to an additional mqtt topic of your choice.
// Leave blank if not needed / unused
constexpr char optionalPublishTopicHeatElementCurrentPower[] = { /*"openWB/LegacySmartHome/set/Devices/2/Aktpower"*/ };

// Possibility to subscribe an additional mqtt topic for triggering the heat pump flag.
// A boolean representation e.g "0" for unset and "1" for set is expected here.
// Leave blank if not needed / unused
constexpr char optionalSubscribeTopicSetPvHeatPumpFlag[] = { /*"openWB/LegacySmartHome/set/Devices/1/ReqRelay"*/ };

// Possibility to subscribe an additional mqtt topic for triggering the heat element flag.
// A boolean representation e.g "0" for unset and "1" for set is expected here.
// Leave blank if not needed / unused
constexpr char optionalSubscribeTopicSetPvHeatElementFlag[] = { /*"openWB/LegacySmartHome/set/Devices/2/ReqRelay"*/ };

}  // namespace mqtt
}  // namespace aquamqtt

#endif  // AQUAMQTT_EXAMPLECONFIGURATION_H
