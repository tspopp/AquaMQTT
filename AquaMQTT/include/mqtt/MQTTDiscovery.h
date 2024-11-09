#ifndef AQUAMQTT_MQTTDISCOVERY_H
#define AQUAMQTT_MQTTDISCOVERY_H

#include <ArduinoJson.h>

#include "MQTTDefinitions.h"
#include "Version.h"

namespace aquamqtt
{
namespace discovery
{
enum class MQTT_ITEM_SENSOR
{
    MAIN_ERROR_CODE,
    MAIN_HOT_WATER_TEMP,
    MAIN_SUPPLY_AIR_TEMP,
    MAIN_EVAPORATOR_AIR_TEMP_UPPER,
    MAIN_EVAPORATOR_AIR_TEMP_LOWER,
    MAIN_FAN_PWM,
    MAIN_SETTING_PWM_01,
    MAIN_SETTING_PWM_02,
    MAIN_SETTING_PWM_03,
    MAIN_SETTING_MIN_TEMP_TARGET,
    MAIN_SETTING_MIN_TEMP_LEGIONELLA,
    MAIN_SETTING_PWR_HEATELEM,
    MAIN_SETTING_BOILER_CAP,
    MAIN_SETTING_BOILER_BRAND,
    HMI_TIME,
    HMI_DATE,
    HMI_LEGIONELLA,
    HMI_TIMER_WINDOW_A,
    HMI_TIMER_WINDOW_B,
    HMI_AIR_DUCT_CONFIG_DEPRECATED,
    HMI_TEST_MODE,
    HMI_SETUP_STATE,
    HMI_PV_INPUT_ACTIVATED,
    ENERGY_TOTAL_HEATING_ELEM_HOURS,
    ENERGY_TOTAL_HEATPUMP_HOURS,
    ENERGY_TOTAL_HOURS,
    ENERGY_TOTAL_ENERGY_WH,
    ENERGY_POWER_TOTAL,
    ENERGY_POWER_HEAT_ELEMENT,
    ENERGY_POWER_HEATPUMP,
    ENERGY_TOTAL_WATER_PRODUCTION,
    STATS_AQUAMQTT_ADDR,
    STATS_AQUAMQTT_RSSI,
    STATS_AQUAMQTT_MODE,
    STATS_AQUAMQTT_OVERRIDE_MODE,
    STATS_AQUAMQTT_LAST_WILL,
    STATS_LST_MSG_HANDLED,
    STATS_LST_MSG_UNHANDLED,
    STATS_LST_MSG_CRC_NOK,
    STATS_LST_DROPPED_BYTES,
    STATS_HMI_MSG_HANDLED,
    STATS_HMI_MSG_UNHANDLED,
    STATS_HMI_MSG_SENT,
    STATS_HMI_MSG_CRC_NOK,
    STATS_HMI_DROPPED_BYTES,
    STATS_MAIN_MSG_HANDLED,
    STATS_MAIN_MSG_UNHANDLED,
    STATS_MAIN_MSG_SENT,
    STATS_MAIN_MSG_CRC_NOK,
    STATS_MAIN_DROPPED_BYTES,
    MAIN_COMPRESSOR_OUTLET_TEMP,
    RESERVED_COUNT
};

enum class MQTT_ITEM_BINARY_SENSOR
{
    MAIN_STATE_FAN,
    MAIN_STATE_HEATPUMP,
    MAIN_STATE_HEAT_ELEMENT,
    MAIN_STATE_EXT_BOILER,
    MAIN_STATE_DEFROST,
    MAIN_STATE_PV,
    MAIN_STATE_SOLAR,
    STATS_ACTIVE_OVERRIDE_HMI_OPERATION_MODE,
    STATS_ACTIVE_OVERRIDE_HMI_OPERATION_TYPE,
    STATS_ACTIVE_OVERRIDE_HMI_TARGET_TEMP,
    STATS_ACTIVE_OVERRIDE_HMI_HEATING_ELEMENT_ENABLED,
    STATS_ACTIVE_OVERRIDE_HMI_EMERGENCY_MODE_ENABLED,
    STATS_ACTIVE_OVERRIDE_HMI_INSTALL_CONFIG,
    STATS_ACTIVE_OVERRIDE_HMI_TIME_DATE,
    STATS_ACTIVE_OVERRIDE_MAIN_SOLAR,
    STATS_ACTIVE_OVERRIDE_MAIN_PV,
    MAIN_CAPABILITY_HEAT_EXC,
    MAIN_CAPABILITY_CIRCULATION,
    MAIN_CAPABILITY_PV_INPUT,
    MAIN_CAPABILITY_EXT_COMM,
    MAIN_CAPABILITY_DRY_HEATING,
    STATS_ACTIVE_OVERRIDE_HMI_FAN_EXHAUST_CONFIG,
    STATS_ACTIVE_OVERRIDE_HMI_AIRDUCT_CONFIG,
    RESERVED_COUNT
};

enum class MQTT_ITEM_BUTTON
{
    RESET_OVERRIDES,
    RESERVED_COUNT
};

enum class MQTT_ITEM_NUMBER
{
    HMI_HOT_WATER_TEMP_TARGET,
    RESERVED_COUNT
};

enum class MQTT_ITEM_SWITCH
{
    STATS_ENABLE_FLAG_PV_HEATPUMP,
    STATS_ENABLE_FLAG_PV_HEATELEMENT,
    HMI_EMERGENCY_MODE,
    HMI_HEATING_ELEMENT_ENABLED,
    RESERVED_COUNT
};

enum class MQTT_ITEM_SELECT
{
    HMI_OPERATION_MODE,
    HMI_OPERATION_TYPE,
    HMI_INSTALLATION_CONFIG,
    HMI_FAN_EXHAUST_CONFIG,
    HMI_AIR_DUCT_CONFIG,
    RESERVED_COUNT
};

enum class MQTT_ITEM_WATER_HEATER
{
    WATER_HEATER_CUSTOM,
    RESERVED_COUNT
};

static JsonDocument defaultJson;

static JsonDocument createFromDefault(uint16_t identifier)
{
    // lazy init
    if (defaultJson.size() == 0)
    {
        char baseTopic[strlen(config::mqttPrefix) + 10];
        sprintf(baseTopic, "%saquamqtt", config::mqttPrefix);
        defaultJson["~"]           = baseTopic;
        defaultJson["dev"]["ids"]  = identifier;
        defaultJson["dev"]["mf"]   = "Groupe Atlantic";
        defaultJson["dev"]["name"] = "AquaMQTT DHW Heat Pump";
        defaultJson["dev"]["mdl"]  = config::heatpumpModelName;
        defaultJson["o"]["name"]   = "AquaMQTT";
        defaultJson["o"]["sw"]     = aquamqtt::VERSION;
        defaultJson["o"]["url"]    = "https://github.com/tspopp/AquaMQTT";
    }

    return { defaultJson };
}

static const char* make_unique(char* buffer, uint16_t identifier, const char* str)
{
    sprintf(buffer, "amq%u_%s", identifier, str);
    return buffer;
}

static bool buildConfiguration(uint8_t* buffer, uint16_t identifier, MQTT_ITEM_SENSOR item)
{
    JsonDocument doc = createFromDefault(identifier);
    char         temp[100];
    switch (item)
    {
        case MQTT_ITEM_SENSOR::MAIN_HOT_WATER_TEMP:
            doc["name"]         = "Water Temperature";
            doc["stat_t"]       = "~/main/waterTemp";
            doc["stat_cla"]     = "measurement";
            doc["unit_of_meas"] = "°C";
            doc["ic"]           = "mdi:thermometer-water";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_waterTemp");
            break;
        case MQTT_ITEM_SENSOR::MAIN_SUPPLY_AIR_TEMP:
            doc["name"]         = "Supply Air Temperature";
            doc["stat_t"]       = "~/main/supplyAirTemp";
            doc["stat_cla"]     = "measurement";
            doc["unit_of_meas"] = "°C";
            doc["ic"]           = "mdi:thermometer";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_supplyAirTemp");
            break;
        case MQTT_ITEM_SENSOR::MAIN_EVAPORATOR_AIR_TEMP_UPPER:
            doc["name"]         = "Upper Evaporator Air Temperature";
            doc["stat_t"]       = "~/main/evaporatorAirTemp";
            doc["stat_cla"]     = "measurement";
            doc["unit_of_meas"] = "°C";
            doc["ic"]           = "mdi:thermometer";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_evoAirTempUpper");
            break;
        case MQTT_ITEM_SENSOR::MAIN_EVAPORATOR_AIR_TEMP_LOWER:
            doc["name"]         = "Lower Evaporator Air Temperature";
            doc["stat_t"]       = "~/main/evaporatorAirTempLower";
            doc["stat_cla"]     = "measurement";
            doc["unit_of_meas"] = "°C";
            doc["ic"]           = "mdi:thermometer";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_evoAirTempLower");
            break;
        case MQTT_ITEM_SENSOR::MAIN_COMPRESSOR_OUTLET_TEMP:
            // FIXME: only if new protocol is detected
            doc["name"]         = "Compressor Outlet Temp";
            doc["stat_t"]       = "~/main/compressorOutletTemp";
            doc["stat_cla"]     = "measurement";
            doc["unit_of_meas"] = "°C";
            doc["ic"]           = "mdi:thermometer";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_compressorOutletTemp");
            break;

        case MQTT_ITEM_SENSOR::MAIN_FAN_PWM:
            doc["name"]         = "Fan PWM";
            doc["stat_t"]       = "~/main/fanPWM";
            doc["unit_of_meas"] = "%";
            doc["ic"]           = "mdi:fan";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_fanpwm");
            break;
        case MQTT_ITEM_SENSOR::STATS_AQUAMQTT_LAST_WILL:
            doc["name"]    = "Status";
            doc["stat_t"]  = "~/stats/lwlState";
            doc["ic"]      = "mdi:information";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_lwl");
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_AQUAMQTT_ADDR:
            doc["name"]    = "IP Address";
            doc["stat_t"]  = "~/stats/ipAddress";
            doc["ic"]      = "mdi:ip-network";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_ip");
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_AQUAMQTT_RSSI:
            doc["name"]         = "RSSI";
            doc["stat_t"]       = "~/stats/rssiDb";
            doc["stat_cla"]     = "measurement";
            doc["unit_of_meas"] = "dB";
            doc["ic"]           = "mdi:signal-cellular-2";
            doc["uniq_id"]      = make_unique(temp, identifier, "stats_rssi");
            doc["ent_cat"]      = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::MAIN_ERROR_CODE:
            doc["name"]    = "Error Code";
            doc["stat_t"]  = "~/main/errorCode";
            doc["ic"]      = "mdi:alarm-light";
            doc["uniq_id"] = make_unique(temp, identifier, "main_error_code");
            break;
        case MQTT_ITEM_SENSOR::ENERGY_TOTAL_HEATING_ELEM_HOURS:
            doc["name"]         = "Total Heating Element Hours";
            doc["stat_t"]       = "~/energy/totalHeatingElemHours";
            doc["unit_of_meas"] = "h";
            doc["stat_cla"]     = "total";
            doc["ic"]           = "mdi:counter";
            doc["uniq_id"]      = make_unique(temp, identifier, "energy_total_he_h");
            break;
        case MQTT_ITEM_SENSOR::ENERGY_TOTAL_HEATPUMP_HOURS:
            doc["name"]         = "Total Heat Pump Hours";
            doc["stat_t"]       = "~/energy/totalHeatpumpHours";
            doc["unit_of_meas"] = "h";
            doc["stat_cla"]     = "total";
            doc["ic"]           = "mdi:counter";
            doc["uniq_id"]      = make_unique(temp, identifier, "energy_total_hp_h");
            break;
        case MQTT_ITEM_SENSOR::ENERGY_TOTAL_HOURS:
            doc["name"]         = "Total Hours";
            doc["stat_t"]       = "~/energy/totalHours";
            doc["unit_of_meas"] = "h";
            doc["stat_cla"]     = "total";
            doc["ic"]           = "mdi:counter";
            doc["uniq_id"]      = make_unique(temp, identifier, "energy_total_h");
            break;
        case MQTT_ITEM_SENSOR::ENERGY_TOTAL_ENERGY_WH:
            doc["name"]         = "Total Energy";
            doc["stat_t"]       = "~/energy/totalEnergyWh";
            doc["unit_of_meas"] = "Wh";
            doc["stat_cla"]     = "total";
            doc["ic"]           = "mdi:lightning-bolt-circle";
            doc["uniq_id"]      = make_unique(temp, identifier, "energy_total_energy");
            doc["dev_cla"]      = "energy";
            break;
        case MQTT_ITEM_SENSOR::ENERGY_POWER_TOTAL:
            doc["name"]         = "Power Consumed Total";
            doc["stat_t"]       = "~/energy/powerTotal";
            doc["unit_of_meas"] = "W";
            doc["stat_cla"]     = "measurement";
            doc["ic"]           = "mdi:lightning-bolt-outline";
            doc["uniq_id"]      = make_unique(temp, identifier, "energy_power_total");
            doc["dev_cla"]      = "power";
            break;
        case MQTT_ITEM_SENSOR::ENERGY_POWER_HEAT_ELEMENT:
            doc["name"]         = "Power Consumed Heating Element";
            doc["stat_t"]       = "~/energy/powerHeatingElem";
            doc["unit_of_meas"] = "W";
            doc["stat_cla"]     = "measurement";
            doc["ic"]           = "mdi:lightning-bolt";
            doc["uniq_id"]      = make_unique(temp, identifier, "energy_power_he");
            doc["dev_cla"]      = "power";
            break;
        case MQTT_ITEM_SENSOR::ENERGY_POWER_HEATPUMP:
            doc["name"]         = "Power Consumed Heatpump";
            doc["stat_t"]       = "~/energy/powerHeatpump";
            doc["unit_of_meas"] = "W";
            doc["stat_cla"]     = "measurement";
            doc["ic"]           = "mdi:lightning-bolt";
            doc["uniq_id"]      = make_unique(temp, identifier, "energy_power_hp");
            doc["dev_cla"]      = "power";
            break;
        case MQTT_ITEM_SENSOR::ENERGY_TOTAL_WATER_PRODUCTION:
            doc["name"]         = "Total Water Production";
            doc["stat_t"]       = "~/energy/totalWaterProduction";
            doc["unit_of_meas"] = "l";
            doc["stat_cla"]     = "total_increasing";
            doc["ic"]           = "mdi:water-pump";
            doc["uniq_id"]      = make_unique(temp, identifier, "energy_total_wp");
            break;
        case MQTT_ITEM_SENSOR::STATS_HMI_MSG_HANDLED:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "HMI Messages Handled";
            doc["stat_t"]  = "~/stats/hmi/msgHandled";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_hmi_handled");
            doc["ic"]      = "mdi:message-arrow-left";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_HMI_MSG_UNHANDLED:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "HMI Messages Unhandled";
            doc["stat_t"]  = "~/stats/hmi/msgUnhandled";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_hmi_unhandled");
            doc["ic"]      = "mdi:message-bulleted-off";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_HMI_MSG_SENT:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "HMI Messages Sent";
            doc["stat_t"]  = "~/stats/hmi/msgSent";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_hmi_sent");
            doc["ic"]      = "mdi:message-arrow-right";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_HMI_MSG_CRC_NOK:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "HMI Messages CRC NOK";
            doc["stat_t"]  = "~/stats/hmi/msgCRCNOK";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_hmi_crcnok");
            doc["ic"]      = "mdi:message-alert";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_HMI_DROPPED_BYTES:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "HMI Bytes Dropped";
            doc["stat_t"]  = "~/stats/hmi/droppedBytes";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_hmi_dropped");
            doc["ic"]      = "mdi:trash-can";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_MAIN_MSG_HANDLED:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "MAIN Messages Handled";
            doc["stat_t"]  = "~/stats/main/msgHandled";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_main_handled");
            doc["ic"]      = "mdi:message-arrow-left";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_MAIN_MSG_UNHANDLED:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "MAIN Messages Unhandled";
            doc["stat_t"]  = "~/stats/main/msgUnhandled";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_main_unhandled");
            doc["ic"]      = "mdi:message-bulleted-off";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_MAIN_MSG_SENT:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "MAIN Messages Sent";
            doc["stat_t"]  = "~/stats/main/msgSent";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_main_sent");
            doc["ic"]      = "mdi:message-arrow-right";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_MAIN_MSG_CRC_NOK:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "MAIN Messages CRC NOK";
            doc["stat_t"]  = "~/stats/main/msgCRCNOK";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_main_crcnok");
            doc["ic"]      = "mdi:message-alert";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_MAIN_DROPPED_BYTES:
            if (config::OPERATION_MODE != config::EOperationMode::MITM || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "MAIN Bytes Dropped";
            doc["stat_t"]  = "~/stats/main/droppedBytes";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_main_dropped");
            doc["ic"]      = "mdi:trash-can";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_LST_MSG_HANDLED:
            if (config::OPERATION_MODE != config::EOperationMode::LISTENER || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "Messages Handled";
            doc["stat_t"]  = "~/stats/msgHandled";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_lst_handled");
            doc["ic"]      = "mdi:message-arrow-left";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_LST_MSG_UNHANDLED:
            if (config::OPERATION_MODE != config::EOperationMode::LISTENER || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "Messages Unhandled";
            doc["stat_t"]  = "~/stats/msgUnhandled";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_lst_unhandled");
            doc["ic"]      = "mdi:message-bulleted-off";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_LST_MSG_CRC_NOK:
            if (config::OPERATION_MODE != config::EOperationMode::LISTENER || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "Messages CRC NOK";
            doc["stat_t"]  = "~/stats/msgCRCNOK";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_lst_crcnok");
            doc["ic"]      = "mdi:message-alert";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_LST_DROPPED_BYTES:
            if (config::OPERATION_MODE != config::EOperationMode::LISTENER || !config::MQTT_PUBLISH_SERIAL_STATISTICS)
            {
                return false;
            }
            doc["name"]    = "Bytes Dropped";
            doc["stat_t"]  = "~/stats/droppedBytes";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_lst_dropped");
            doc["ic"]      = "mdi:trash-can";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::HMI_TIME:
            if (!config::MQTT_PUBLISH_HEATPUMP_TIME_AND_DATE)
            {
                return false;
            }
            doc["name"]    = "Time";
            doc["stat_t"]  = "~/hmi/time";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_time");
            doc["ic"]      = "mdi:clock-time-eight";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::HMI_DATE:
            if (!config::MQTT_PUBLISH_HEATPUMP_TIME_AND_DATE)
            {
                return false;
            }
            doc["name"]    = "Date";
            doc["stat_t"]  = "~/hmi/date";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_date");
            doc["ic"]      = "mdi:calendar-range";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::HMI_LEGIONELLA:
            doc["name"]    = "Anti-Legionella cycles per month";
            doc["stat_t"]  = "~/hmi/antiLegionellaPerMonth";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_legionella");
            doc["ic"]      = "mdi:virus-off";
            break;
        case MQTT_ITEM_SENSOR::HMI_TIMER_WINDOW_A:
            doc["name"]    = "Timer Window A";
            doc["stat_t"]  = "~/hmi/timerWindowA";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_win_a");
            doc["ic"]      = "mdi:calendar-clock";
            break;
        case MQTT_ITEM_SENSOR::HMI_TIMER_WINDOW_B:
            doc["name"]    = "Timer Window B";
            doc["stat_t"]  = "~/hmi/timerWindowB";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_win_b");
            doc["ic"]      = "mdi:calendar-clock";
            break;
        case MQTT_ITEM_SENSOR::HMI_SETUP_STATE:
            doc["name"]    = "Setup State";
            doc["stat_t"]  = "~/hmi/setupState";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_setup");
            doc["ic"]      = "mdi:tablet-dashboard";
            break;
        case MQTT_ITEM_SENSOR::HMI_AIR_DUCT_CONFIG_DEPRECATED:
            return false;
        case MQTT_ITEM_SENSOR::STATS_AQUAMQTT_MODE:
            doc["name"]    = "AquaMQTT Mode";
            doc["stat_t"]  = "~/stats/aquamqttMode";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_amq_mode");
            doc["ic"]      = "mdi:heat-pump";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::STATS_AQUAMQTT_OVERRIDE_MODE:
            doc["name"]    = "AquaMQTT Override Mode";
            doc["stat_t"]  = "~/stats/overrideMode";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_amq_overridemode");
            doc["ic"]      = "mdi:heat-pump";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_SENSOR::MAIN_SETTING_PWM_01:
            doc["name"]         = "PWM Fan Level 1 ";
            doc["stat_t"]       = "~/main/settingPWM_1";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_setting_fan1");
            doc["unit_of_meas"] = "%";
            doc["ic"]           = "mdi:fan-speed-1";
            break;
        case MQTT_ITEM_SENSOR::MAIN_SETTING_PWM_02:
            doc["name"]         = "PWM Fan Level 2 ";
            doc["stat_t"]       = "~/main/settingPWM_2";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_setting_fan2");
            doc["unit_of_meas"] = "%";
            doc["ic"]           = "mdi:fan-speed-2";
            break;
        case MQTT_ITEM_SENSOR::MAIN_SETTING_PWM_03:
            doc["name"]         = "PWM Fan Level 3 ";
            doc["stat_t"]       = "~/main/settingPWM_3";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_setting_fan3");
            doc["unit_of_meas"] = "%";
            doc["ic"]           = "mdi:fan-speed-3";
            break;
        case MQTT_ITEM_SENSOR::MAIN_SETTING_MIN_TEMP_TARGET:
            doc["name"]         = "Minimum Water Target Temperature";
            doc["stat_t"]       = "~/main/settingMinTargetTemp";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_setting_min_target_temp");
            doc["unit_of_meas"] = "°C";
            doc["ic"]           = "mdi:thermometer-chevron-down";
            break;
        case MQTT_ITEM_SENSOR::MAIN_SETTING_MIN_TEMP_LEGIONELLA:
            doc["name"]         = "Minimum Water Temperature Anti-Legionella";
            doc["stat_t"]       = "~/main/settingMinLegionellaTemp";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_setting_min_target_legionella_temp");
            doc["unit_of_meas"] = "°C";
            doc["ic"]           = "mdi:thermometer-plus";
            break;
        case MQTT_ITEM_SENSOR::MAIN_SETTING_PWR_HEATELEM:
            doc["name"]         = "Wattage Installed Heat Element";
            doc["stat_t"]       = "~/main/settingWattageElement";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_setting_wattage_he");
            doc["unit_of_meas"] = "W";
            doc["ic"]           = "mdi:heating-coil";
            break;
        case MQTT_ITEM_SENSOR::MAIN_SETTING_BOILER_CAP:
            doc["name"]         = "Boiler Capacity";
            doc["stat_t"]       = "~/main/settingBoilerCapacity";
            doc["uniq_id"]      = make_unique(temp, identifier, "main_setting_cap");
            doc["unit_of_meas"] = "l";
            doc["ic"]           = "mdi:barrel";
            break;
        case MQTT_ITEM_SENSOR::MAIN_SETTING_BOILER_BRAND:
            doc["name"]    = "Boiler Brand";
            doc["stat_t"]  = "~/main/settingBoilerBrand";
            doc["uniq_id"] = make_unique(temp, identifier, "main_setting_brand");
            doc["ic"]      = "mdi:factory";
            break;
        case MQTT_ITEM_SENSOR::HMI_TEST_MODE:
            doc["name"]    = "Test Mode Status";
            doc["stat_t"]  = "~/hmi/testModeStatus";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_test_mode");
            doc["ic"]      = "mdi:test-tube";
            break;
        case MQTT_ITEM_SENSOR::HMI_PV_INPUT_ACTIVATED:
            doc["name"]    = "PV Input allowed";
            doc["stat_t"]  = "~/hmi/pvInputActivated";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_pv_allowed");
            doc["ic"]      = "mdi:solar-power-variant";
        case MQTT_ITEM_SENSOR::RESERVED_COUNT:
        default:
            return false;

    }

    serializeJson(doc, buffer, config::MQTT_MAX_PAYLOAD_SIZE);
    return true;
}

static bool buildConfiguration(uint8_t* buffer, uint16_t identifier, MQTT_ITEM_BINARY_SENSOR item)
{
    JsonDocument doc = createFromDefault(identifier);
    char         temp[100];
    switch (item)
    {
        case MQTT_ITEM_BINARY_SENSOR::MAIN_STATE_FAN:
            doc["name"]    = "Fan";
            doc["stat_t"]  = "~/main/stateFan";
            doc["ic"]      = "mdi:fan";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_fan");
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_STATE_HEATPUMP:
            doc["name"]    = "Heatpump";
            doc["stat_t"]  = "~/main/stateHeatpump";
            doc["ic"]      = "mdi:heat-pump-outline";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_heatpump");
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_STATE_HEAT_ELEMENT:
            doc["name"]    = "Heating Element";
            doc["stat_t"]  = "~/main/stateElement";
            doc["ic"]      = "mdi:heating-coil";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_heatelement");
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_STATE_EXT_BOILER:
            doc["name"]    = "External Boiler";
            doc["stat_t"]  = "~/main/stateExtBoiler";
            doc["ic"]      = "mdi:water-boiler";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_extboiler");
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_STATE_DEFROST:
            doc["name"]    = "Defrost";
            doc["stat_t"]  = "~/main/stateDefrost";
            doc["ic"]      = "mdi:snowflake-melt";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_defrost");
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_STATE_PV:
            doc["name"]    = "PV";
            doc["stat_t"]  = "~/main/statePV";
            doc["ic"]      = "mdi:solar-power-variant";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_pv");
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_STATE_SOLAR:
            doc["name"]    = "Solar";
            doc["stat_t"]  = "~/main/stateSolar";
            doc["ic"]      = "mdi:solar-power-variant-outline";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_solar");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_OPERATION_MODE:
            doc["name"]    = "Active Override Operation Mode";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json.operationMode }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_om");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_OPERATION_TYPE:
            doc["name"]    = "Active Override Operation Type";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json.operationType }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_ot");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_TARGET_TEMP:
            doc["name"]    = "Active Override Target Temperature";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json.waterTempTarget }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_targetTemp");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_HEATING_ELEMENT_ENABLED:
            doc["name"]    = "Active Override Heating Element";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json.heatingElementEnabled }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_he_enabled");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_EMERGENCY_MODE_ENABLED:
            doc["name"]    = "Active Override Emergency Mode";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json.emergencyModeEnabled }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_em_enabled");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_INSTALL_CONFIG:
            doc["name"]    = "Active Override Configuration Installation";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json.configInstallation }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_inst_cfg");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_FAN_EXHAUST_CONFIG:
            doc["name"]    = "Active Override Configuration Fan Exhaust";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json.configFanExhaust }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_fan_exh_cfg");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_AIRDUCT_CONFIG:
            doc["name"]    = "Active Override Configuration Air Duct";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json.configAirduct }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_airduct_cfg");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_HMI_TIME_DATE:
            doc["name"]    = "Active Override Time/Date";
            doc["stat_t"]  = "~/stats/activeOverrides";
            doc["val_tpl"] = "{{ value_json['time/date'] }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_time");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_MAIN_SOLAR:
            doc["name"]    = "Active Override Solar Input";
            doc["stat_t"]  = "~/stats/activeOverridesMain";
            doc["val_tpl"] = "{{ value_json.stateSolar }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_solar");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::STATS_ACTIVE_OVERRIDE_MAIN_PV:
            doc["name"]    = "Active Override PV Input";
            doc["stat_t"]  = "~/stats/activeOverridesMain";
            doc["val_tpl"] = "{{ value_json.statePV }}";
            doc["ic"]      = "mdi:debug-step-over";
            doc["uniq_id"] = make_unique(temp, identifier, "main_state_override_pv");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_CAPABILITY_HEAT_EXC:
            doc["name"]    = "Capability Heat-Exchanger";
            doc["stat_t"]  = "~/main/capabilityHeatExchanger";
            doc["ic"]      = "mdi:expansion-card";
            doc["uniq_id"] = make_unique(temp, identifier, "main_cap_heat_exc");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_CAPABILITY_CIRCULATION:
            doc["name"]    = "Capability Circulation";
            doc["stat_t"]  = "~/main/capabilityCirculation";
            doc["ic"]      = "mdi:expansion-card";
            doc["uniq_id"] = make_unique(temp, identifier, "main_cap_circ");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_CAPABILITY_PV_INPUT:
            doc["name"]    = "Capability PV Input";
            doc["stat_t"]  = "~/main/capabilityPVInput";
            doc["ic"]      = "mdi:expansion-card";
            doc["uniq_id"] = make_unique(temp, identifier, "main_cap_pv_in");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_CAPABILITY_EXT_COMM:
            doc["name"]    = "Capability External Communication";
            doc["stat_t"]  = "~/main/capabilityCommunication";
            doc["ic"]      = "mdi:expansion-card";
            doc["uniq_id"] = make_unique(temp, identifier, "main_cap_com");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::MAIN_CAPABILITY_DRY_HEATING:
            doc["name"]    = "Capability Anti Dry Heating";
            doc["stat_t"]  = "~/main/capabilityCommunication";
            doc["ic"]      = "mdi:expansion-card";
            doc["uniq_id"] = make_unique(temp, identifier, "main_cap_anti_dry_heat");
            doc["pl_on"]   = "1";
            doc["pl_off"]  = "0";
            doc["ent_cat"] = "diagnostic";
            break;
        case MQTT_ITEM_BINARY_SENSOR::RESERVED_COUNT:
        default:
            return false;
    }

    serializeJson(doc, buffer, config::MQTT_MAX_PAYLOAD_SIZE);
    return true;
}

static bool buildConfiguration(uint8_t* buffer, uint16_t identifier, MQTT_ITEM_NUMBER item)
{
    JsonDocument doc = createFromDefault(identifier);
    char         temp[100];
    switch (item)
    {
        case MQTT_ITEM_NUMBER::HMI_HOT_WATER_TEMP_TARGET:
            doc["name"]         = "Target Temperature";
            doc["stat_t"]       = "~/hmi/waterTempTarget";
            doc["cmd_t"]        = "~/ctrl/waterTempTarget";
            doc["ic"]           = "mdi:thermometer-water";
            doc["uniq_id"]      = make_unique(temp, identifier, "hmi_waterTempTarget");
            doc["ent_cat"]      = "config";
            doc["unit_of_meas"] = "°C";
            doc["min"]          = config::ABSENCE_WATER_TEMPERATURE;
            doc["max"]          = config::MAX_WATER_TEMPERATURE;
            doc["step"]         = "1";
            break;
        case MQTT_ITEM_NUMBER::RESERVED_COUNT:
        default:
            return false;
    }

    serializeJson(doc, buffer, config::MQTT_MAX_PAYLOAD_SIZE);
    return true;
}

static bool buildConfiguration(uint8_t* buffer, uint16_t identifier, MQTT_ITEM_BUTTON item)
{
    JsonDocument doc = createFromDefault(identifier);
    char         temp[100];
    switch (item)
    {
        case MQTT_ITEM_BUTTON::RESET_OVERRIDES:
            doc["name"]    = "Reset Overrides";
            doc["cmd_t"]   = "~/ctrl/reset";
            doc["ic"]      = "mdi:restart";
            doc["uniq_id"] = make_unique(temp, identifier, "reset_overrides");
            doc["ent_cat"] = "config";
            doc["pl_prs"]  = "";
            break;
        case MQTT_ITEM_BUTTON::RESERVED_COUNT:
        default:
            return false;
    }

    serializeJson(doc, buffer, config::MQTT_MAX_PAYLOAD_SIZE);
    return true;
}

static bool buildConfiguration(uint8_t* buffer, uint16_t identifier, MQTT_ITEM_SELECT item)
{
    JsonDocument doc = createFromDefault(identifier);
    char         temp[100];
    switch (item)
    {
        case MQTT_ITEM_SELECT::HMI_OPERATION_MODE:
            doc["name"]    = "Operation Mode";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_opMode");
            doc["stat_t"]  = "~/hmi/operationMode";
            doc["cmd_t"]   = "~/ctrl/operationMode";
            doc["ic"]      = "mdi:list-box";
            doc["ops"][0]  = mqtt::ENUM_OPERATION_MODE_ABSENCE;
            doc["ops"][1]  = mqtt::ENUM_OPERATION_MODE_AUTO;
            doc["ops"][2]  = mqtt::ENUM_OPERATION_MODE_BOOST;
            doc["ops"][3]  = mqtt::ENUM_OPERATION_MODE_ECO_OFF;
            doc["ops"][4]  = mqtt::ENUM_OPERATION_MODE_ECO_ON;
            break;
        case MQTT_ITEM_SELECT::HMI_OPERATION_TYPE:
            doc["name"]    = "Operation Type";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_opType");
            doc["stat_t"]  = "~/hmi/operationType";
            doc["cmd_t"]   = "~/ctrl/operationType";
            doc["ic"]      = "mdi:timetable";
            doc["ops"][0]  = mqtt::ENUM_OPERATION_TYPE_ALWAYS_ON;
            doc["ops"][1]  = mqtt::ENUM_OPERATION_TYPE_TIMER;
            break;
        case MQTT_ITEM_SELECT::HMI_INSTALLATION_CONFIG:
            doc["name"]    = "Installation Configuration";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_instConfig");
            doc["stat_t"]  = "~/hmi/configInstallation";
            doc["cmd_t"]   = "~/ctrl/configInstallation";
            doc["ic"]      = "mdi:cog";
            doc["ops"][0]  = mqtt::ENUM_INSTALLATION_BOILER_BACKUP_EXT_OPT;
            doc["ops"][1]  = mqtt::ENUM_INSTALLATION_BOILER_BACKUP_EXT_PRIO;
            doc["ops"][2]  = mqtt::ENUM_INSTALLATION_BOILER_BACKUP_HP_PRIO;
            doc["ops"][3]  = mqtt::ENUM_INSTALLATION_BOILER_BACKUP_HP_OPT;
            doc["ops"][4]  = mqtt::ENUM_INSTALLATION_THERMODYNAMICS_ONLY;
            doc["ops"][5]  = mqtt::ENUM_INSTALLATION_SOLAR_BACKUP;
            break;
        case MQTT_ITEM_SELECT::HMI_FAN_EXHAUST_CONFIG:
            doc["name"]    = "Fan Exhaust Configuration";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_fanExhaustConfig");
            doc["stat_t"]  = "~/hmi/configFanExhaust";
            doc["cmd_t"]   = "~/ctrl/configFanExhaust";
            doc["ic"]      = "mdi:fan";
            doc["ops"][0]  = mqtt::ENUM_CONFIG_EXHAUST_FAN_STOP;
            doc["ops"][1]  = mqtt::ENUM_CONFIG_EXHAUST_FAN_LOW_SPEED;
            doc["ops"][2]  = mqtt::ENUM_CONFIG_EXHAUST_FAN_HIGH_SPEED;
            break;
        case MQTT_ITEM_SELECT::HMI_AIR_DUCT_CONFIG:
            doc["name"]    = "AirDuct Configuration";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_airduct_cfg_sel");
            doc["stat_t"]  = "~/hmi/configAirduct";
            doc["cmd_t"]   = "~/ctrl/configAirduct";
            doc["ic"]      = "mdi:sign-direction";
            doc["ops"][0]  = mqtt::ENUM_AIR_DUCT_IN_IN;
            doc["ops"][1]  = mqtt::ENUM_AIR_DUCT_INT_EXT;
            doc["ops"][2]  = mqtt::ENUM_AIR_DUCT_EXT_EXT;
            break;
        case MQTT_ITEM_SELECT::RESERVED_COUNT:
        default:
            return false;
    }

    serializeJson(doc, buffer, config::MQTT_MAX_PAYLOAD_SIZE);
    return true;
}

static bool buildConfiguration(uint8_t* buffer, uint16_t identifier, MQTT_ITEM_SWITCH item)
{
    JsonDocument doc = createFromDefault(identifier);
    char         temp[100];
    switch (item)
    {
        case MQTT_ITEM_SWITCH::STATS_ENABLE_FLAG_PV_HEATPUMP:
            doc["name"]    = "PV Mode Heat Pump";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_flag_pv_hp");
            doc["stat_t"]  = "~/stats/flagPVModeHeatPump";
            doc["cmd_t"]   = "~/ctrl/flagPVModeHeatPump";
            doc["ic"]      = "mdi:heat-pump";
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_SWITCH::STATS_ENABLE_FLAG_PV_HEATELEMENT:
            doc["name"]    = "PV Mode Heating Element";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "stats_flag_pv_he");
            doc["stat_t"]  = "~/stats/flagPVModeHeatElement";
            doc["cmd_t"]   = "~/ctrl/flagPVModeHeatElement";
            doc["ic"]      = "mdi:heating-coil";
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_SWITCH::HMI_EMERGENCY_MODE:
            doc["name"]    = "Emergency Mode (Disabled Heatpump)";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_emergencyMode");
            doc["stat_t"]  = "~/hmi/emergencyModeEnabled";
            doc["cmd_t"]   = "~/ctrl/emergencyModeEnabled";
            doc["ic"]      = "mdi:car-brake-alert";
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        case MQTT_ITEM_SWITCH::HMI_HEATING_ELEMENT_ENABLED:
            doc["name"]    = "Allow Heating Element";
            doc["ent_cat"] = "config";
            doc["uniq_id"] = make_unique(temp, identifier, "hmi_he_enabled");
            doc["stat_t"]  = "~/hmi/heatingElementEnabled";
            doc["cmd_t"]   = "~/ctrl/heatingElementEnabled";
            doc["ic"]      = "mdi:heating-coil";
            doc["pl_off"]  = "0";
            doc["pl_on"]   = "1";
            break;
        default:
            return false;
    }

    serializeJson(doc, buffer, config::MQTT_MAX_PAYLOAD_SIZE);
    return true;
}

static bool buildConfiguration(uint8_t* buffer, uint16_t identifier, MQTT_ITEM_WATER_HEATER item)
{
    JsonDocument doc = createFromDefault(identifier);
    char         temp[100];
    switch (item)
    {
        case MQTT_ITEM_WATER_HEATER::WATER_HEATER_CUSTOM:
            doc["name"]             = config::heatpumpModelName;
            doc["ent_cat"]          = "config";
            doc["uniq_id"]          = make_unique(temp, identifier, "water_heater");
            doc["ic"]               = "mdi:water";
            doc["mode_stat_t"]      = "~/hmi/operationMode";
            doc["mode_cmd_t"]       = "~/ctrl/operationMode";
            doc["modes"][0]         = mqtt::ENUM_OPERATION_MODE_AUTO;
            doc["modes"][1]         = mqtt::ENUM_OPERATION_MODE_BOOST;
            doc["modes"][2]         = mqtt::ENUM_OPERATION_MODE_ECO_ON;
            doc["modes"][3]         = mqtt::ENUM_OPERATION_MODE_ECO_OFF;
            doc["modes"][4]         = mqtt::ENUM_OPERATION_MODE_ABSENCE;
            doc["min"]              = config::ABSENCE_WATER_TEMPERATURE;
            doc["max"]              = config::MAX_WATER_TEMPERATURE;
            doc["temp_stat_t"]      = "~/hmi/waterTempTarget";
            doc["temp_cmd_t"]       = "~/ctrl/waterTempTarget";
            doc["curr_temp_t"]      = "~/main/waterTemp";
            doc["precision"]        = 1.0f;
            doc["temperature_unit"] = "C";
            break;
        default:
            return false;
    }

    serializeJson(doc, buffer, config::MQTT_MAX_PAYLOAD_SIZE);
    return true;
}

}  // namespace discovery
}  // namespace aquamqtt

#endif  // AQUAMQTT_MQTTDISCOVERY_H
