#ifndef AQUAMQTT_CONFIGURATION_H
#define AQUAMQTT_CONFIGURATION_H

//#define CUSTOM_CONFIGURATION

#ifdef CUSTOM_CONFIGURATION
#    include "CustomConfiguration.h"
#else
#    include "ExampleConfiguration.h"
#endif

#include "Modes.h"

namespace aquamqtt
{
namespace config
{
constexpr char networkName[] = "aquamqtt";

constexpr char timeZoneInfo[] =
        "CET-1CEST,M3.5.0,M10.5.0/3";  // Europe/Berlin https://gist.github.com/alwynallan/24d96091655391107939
constexpr char ntpServer[] = "pool.ntp.org";

constexpr EOperationMode OPERATION_MODE = EOperationMode::MITM;

constexpr uint8_t  WATCHDOG_TIMEOUT_S    = 60;
constexpr uint16_t MQTT_STATS_UPDATE_MS  = 5000;
constexpr uint8_t  MQTT_MAX_TOPIC_SIZE   = 50;
constexpr uint8_t  MQTT_MAX_PAYLOAD_SIZE = 50;

// AquaMQTT Board Rev 1.0
constexpr uint8_t GPIO_MAIN_RX = 2;
constexpr uint8_t GPIO_MAIN_TX = 3;
constexpr uint8_t GPIO_HMI_RX  = 4;
constexpr uint8_t GPIO_HMI_TX  = 5;

constexpr uint8_t GPIO_SDA_RTC = A4;
constexpr uint8_t GPIO_SCL_RTC = A5;

}  // namespace config
}  // namespace aquamqtt

#endif  // AQUAMQTT_CONFIGURATION_H
