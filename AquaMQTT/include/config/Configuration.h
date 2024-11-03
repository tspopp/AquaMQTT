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

#include "Modes.h"

namespace aquamqtt
{
namespace config
{

/**
 * Defines the network name of your esp32 device in your network
 */
constexpr char networkName[] = "aquamqtt";

/**
 * Choose a timezone which is used for NTP time updates:
 * https://gist.github.com/alwynallan/24d96091655391107939
 */
constexpr char timeZoneInfo[] = "CET-1CEST,M3.5.0,M10.5.0/3";  // Europe/Berlin

/**
 * The NTP Time Server used for fetching the current time
 */
constexpr char ntpServer[] = "pool.ntp.org";

/**
 * Enable HomeAssistant MQTT Discovery Mode
 */
constexpr bool ENABLE_HOMEASSISTANT_DISCOVERY_MODE = true;

/**
 * Homeassistant MQTT Discovery Prefix
 * https://www.home-assistant.io/integrations/mqtt/#discovery-topic
 */
constexpr char haDiscoveryPrefix[] = "homeassistant/";

/**
 * The OperationMode which is used for AquaMqtt. Refer to EOperationMode
 */
constexpr EOperationMode OPERATION_MODE = EOperationMode::MITM;

/**
 * Choose if the time and date values sent to the main controller should override the ones sent by the hmi controller.
 */
constexpr bool OVERRIDE_TIME_AND_DATE_IN_MITM = true;

/**
 * Choose to publish raw messages represented as hex-string on debug mqtt topics
 */
constexpr bool DEBUG_RAW_SERIAL_MESSAGES = false;

/**
 * Choose to publish message statistics from the serial interfaces, if everything
 * works as it should you may disable this. If set to true, this will provide the
 * topics "msgHandled, msgUnhandled, msgCRCNOK and msgSent" topics for each serial
 * channel (hmi/main or listener),
 */
constexpr bool MQTT_PUBLISH_SERIAL_STATISTICS = true;

/**
 * Choose to publish time and date used by the heatpump. This is mainly for debugging
 * if the time and date override from AquaMQTT works as expected. You may want to
 * enable this, if you are customizing the NTP timezone or server or even trying to
 * use the RTC module from the AquaMQTT board.
 */
constexpr bool MQTT_PUBLISH_HEATPUMP_TIME_AND_DATE = true;

/**
 * Change the time interval where all known attributes are re-published to the MQTT broker.
 */
constexpr uint32_t MQTT_FULL_UPDATE_MS = 1000 * 60 * 30;

/**
 * Change the fixed time interval where the attributes published to the stats topic are updated.
 */
constexpr uint16_t MQTT_STATS_UPDATE_MS = 5000;

/**
 * Use kalman filters for removing noise from the temperature values. This reduces updates sent to MQTT.
 * Note: Filters are only applied within the MQTT channel, the communication between HMI and Controller
 * are still using raw temperature values and is not altered.
 */
constexpr bool MQTT_FILTER_TEMPERATURE_NOISE = true;

/**
 * Parametrize kalman filter for reading temperature values
 * Measurement Uncertainty - How much do we expect to our measurement vary
 */

constexpr float KALMAN_MEA_E = 0.1;

/**
 * Parametrize kalman filter for reading temperature values
 * Estimation Uncertainty - Can be initialized with the same value as e_mea since the kalman filter will adjust its
 * value.
 */

constexpr float KALMAN_EST_E = 0.1;

/**
 * Parametrize kalman filter for reading temperature values
 * Process Variance - usually a small number between 0.001 and 1 - how fast your measurement moves.
 */
constexpr float KALMAN_Q = 0.01;

/**
 * Maximum Water Temperature: This value is used by BOOST or PV Modes.
 * Change at your own risk, 62°C is the default value on a Windhager Aquawin Air 3 heat pump
 */
constexpr float MAX_WATER_TEMPERATURE = 62.0;

/**
 * Water Temperature set during mode absence.
 * Change at your own risk, 20°C is the default value on a Windhager Aquawin Air 3 heat pump
 */
constexpr float ABSENCE_WATER_TEMPERATURE = 20.0;

/**
 * Self explanatory internal settings: most probably you don't want to change them.
 */
constexpr uint32_t WATCHDOG_TIMEOUT_MS    = 60000;
constexpr uint16_t WIFI_RECONNECT_CYCLE_S = 10;
constexpr uint16_t MQTT_MAX_TOPIC_SIZE    = 255;
constexpr uint16_t MQTT_MAX_PAYLOAD_SIZE  = 1024;

/**
 * Pin assignments for AquaMQTT Board Rev 1.0
 */
constexpr uint8_t GPIO_MAIN_RX = 2;
constexpr uint8_t GPIO_MAIN_TX = 3;
constexpr uint8_t GPIO_HMI_RX  = 4;
constexpr uint8_t GPIO_HMI_TX  = 5;
constexpr uint8_t GPIO_SDA_RTC = A4;
constexpr uint8_t GPIO_SCL_RTC = A5;

}  // namespace config
}  // namespace aquamqtt

#endif  // AQUAMQTT_CONFIGURATION_H
