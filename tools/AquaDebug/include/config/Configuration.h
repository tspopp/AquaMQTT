#ifndef AQUAMQTT_CONFIGURATION_H
#define AQUAMQTT_CONFIGURATION_H

/**
 * Possibility to include your own configuration file (added to .gitignore)
 */
//#define CUSTOM_CONFIGURATION

#ifdef CUSTOM_CONFIGURATION
#    include "CustomConfiguration.h"
#else
#    include "ExampleConfiguration.h"
#endif

namespace aquamqtt
{
namespace config
{
/**
 * Defines the network name of your esp32 device in your network
 */
constexpr char networkName[] = "aquamqtt";

/**
 * Self explanatory internal settings: most probably you don't want to change them.
 */
constexpr uint32_t  WATCHDOG_TIMEOUT_MS   = 60000;
constexpr uint16_t WIFI_RECONNECT_CYCLE_S = 10;
constexpr uint8_t  MQTT_MAX_TOPIC_SIZE   = 80;
constexpr uint8_t  MQTT_MAX_PAYLOAD_SIZE = 255;

/**
 * Pin assignments for AquaMQTT Board Rev 1.0
 * There are different GPIO ports depending on the hardware.
 * The ENV_DEVKIT_ESP32 macro is set by the platformio environment.
 * By default we use the pin assignments of the Arduino Nano ESP32
 */
#ifdef ENV_DEVKIT_ESP32
constexpr uint8_t GPIO_MAIN_RX = 5;
constexpr uint8_t GPIO_MAIN_TX = 6;
constexpr uint8_t GPIO_HMI_RX  = 7;
constexpr uint8_t GPIO_HMI_TX  = 8;
#else
constexpr uint8_t GPIO_MAIN_RX = 2;
constexpr uint8_t GPIO_MAIN_TX = 3;
constexpr uint8_t GPIO_HMI_RX  = 4;
constexpr uint8_t GPIO_HMI_TX  = 5;
#endif
constexpr uint8_t GPIO_SDA_RTC = A4;
constexpr uint8_t GPIO_SCL_RTC = A5;

}  // namespace config
}  // namespace aquamqtt

#endif  // AQUAMQTT_CONFIGURATION_H
