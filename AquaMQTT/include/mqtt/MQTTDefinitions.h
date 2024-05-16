#ifndef AQUAMQTT_MQTTDEFINITIONS_H
#define AQUAMQTT_MQTTDEFINITIONS_H

#include "pgmspace.h"

namespace aquamqtt
{
namespace mqtt
{
// Base Topics
const char BASE_TOPIC[] PROGMEM    = { "aquamqtt/" };
const char CONTROL_TOPIC[] PROGMEM = { "aquamqtt/ctrl/#" };

const char HMI_SUBTOPIC[] PROGMEM    = { "hmi/" };
const char MAIN_SUBTOPIC[] PROGMEM   = { "main/" };
const char ENERGY_SUBTOPIC[] PROGMEM = { "energy/" };
const char STATS_SUBTOPIC[] PROGMEM  = { "stats/" };

// Enum Types
const char ENUM_UNKNOWN[] PROGMEM = { "UNKNOWN" };

const char ENUM_AQUAMQTT_MODE_LISTENER[] PROGMEM = { "LISTENER" };
const char ENUM_AQUAMQTT_MODE_MITM[] PROGMEM     = { "MITM" };

const char ENUM_BRAND_ATLANTIC[] PROGMEM = { "Atlantic" };
const char ENUM_BRAND_THERMOR[] PROGMEM  = { "Thermor" };
const char ENUM_BRAND_SAUTER[] PROGMEM   = { "Sauter" };
const char ENUM_BRAND_NONAME[] PROGMEM   = { "No Name" };

const char ENUM_OPERATION_MODE_AUTO[] PROGMEM    = { "AUTO" };
const char ENUM_OPERATION_MODE_ECO_OFF[] PROGMEM = { "MAN ECO OFF" };
const char ENUM_OPERATION_MODE_ECO_ON[] PROGMEM  = { "MAN ECO ON" };
const char ENUM_OPERATION_MODE_BOOST[] PROGMEM   = { "BOOST" };
const char ENUM_OPERATION_MODE_ABSENCE[] PROGMEM = { "ABSENCE" };

const char ENUM_AIR_DUCT_IN_IN[] PROGMEM   = { "INT/INT" };
const char ENUM_AIR_DUCT_EXT_EXT[] PROGMEM = { "EXT/EXT" };
const char ENUM_AIR_DUCT_INT_EXT[] PROGMEM = { "INT/EXT" };

const char ENUM_INSTALLATION_THERMODYNAMICS_ONLY[] PROGMEM    = { "HEAT PUMP ONLY" };
const char ENUM_INSTALLATION_BOILER_BACKUP_HP_PRIO[] PROGMEM  = { "BOILER BACKUP / HEAT PUMP PRIORITY" };
const char ENUM_INSTALLATION_BOILER_BACKUP_HP_OPT[] PROGMEM   = { "BOILER BACKUP / HEAT PUMP OPTIMIZED" };
const char ENUM_INSTALLATION_BOILER_BACKUP_EXT_OPT[] PROGMEM  = { "BOILER BACKUP / BOILER OPTIMIZED" };
const char ENUM_INSTALLATION_BOILER_BACKUP_EXT_PRIO[] PROGMEM = { "BOILER BACKUP / BOILER PRIORITY" };
const char ENUM_INSTALLATION_SOLAR_BACKUP[] PROGMEM           = { "HEAT PUMP AND SOLAR BACKUP" };

const char ENUM_TEST_MODE_OFF[] PROGMEM                            = { "OFF" };
const char ENUM_TEST_MODE_IDLE[] PROGMEM                           = { "IDLE" };
const char ENUM_TEST_MODE_ACTIVE_HEAT_PUMP[] PROGMEM               = { "TESTING HEAT PUMP" };
const char ENUM_TEST_MODE_ACTIVE_HEAT_ELEM[] PROGMEM               = { "TESTING HEAT ELEM" };
const char ENUM_TEST_MODE_ACTIVE_FAN_HIGH[] PROGMEM                = { "TESTING FAN HIGH" };
const char ENUM_TEST_MODE_ACTIVE_FAN_LOW[] PROGMEM                 = { "TESTING FAN LOW" };
const char ENUM_TEST_MODE_ACTIVE_DEFROSTING[] PROGMEM              = { "TESTING DEFROSTING" };
const char ENUM_TEST_MODE_ACTIVE_HEAT_PUMP_BOILER_BACKUP[] PROGMEM = { "TESTING HEAT PUMP AND BOILER BACKUP" };

const char ENUM_OPERATION_TYPE_ALWAYS_ON[] PROGMEM = { "ALWAYS ON" };
const char ENUM_OPERATION_TYPE_TIMER[] PROGMEM     = { "TIME WINDOW" };

const char ENUM_LAST_WILL_ONLINE[] PROGMEM  = { "ONLINE" };
const char ENUM_LAST_WILL_OFFLINE[] PROGMEM = { "OFFLINE" };

const char ENUM_SETUP_COMPLETED[] PROGMEM  = { "OK" };
const char ENUM_SETUP_INCOMPLETE[] PROGMEM = { "PARTIAL RESET" };
const char ENUM_SETUP_RESET[] PROGMEM      = { "FACTORY RESET" };

const char ENUM_AQUAMQTT_OVERRIDE_MODE_DEFAULT[] PROGMEM = { "DEFAULT" };
const char ENUM_AQUAMQTT_OVERRIDE_MODE_HP_ONLY[] PROGMEM = { "PV HP" };
const char ENUM_AQUAMQTT_OVERRIDE_MODE_HE_ONLY[] PROGMEM = { "PV HE" };
const char ENUM_AQUAMQTT_OVERRIDE_MODE_PV_FULL[] PROGMEM = { "PV BOOST" };

// Subtopics
const char MAIN_HOT_WATER_TEMP[] PROGMEM            = { "waterTemp" };
const char MAIN_SUPPLY_AIR_TEMP[] PROGMEM           = { "supplyAirTemp" };
const char MAIN_EVAPORATOR_AIR_TEMP_UPPER[] PROGMEM = { "evaporatorAirTemp" };
const char MAIN_EVAPORATOR_AIR_TEMP_LOWER[] PROGMEM = { "evaporatorAirTempLower" };
const char MAIN_FAN_SPEED[] PROGMEM                 = { "fanSpeed" };
const char MAIN_STATE_FAN[] PROGMEM                 = { "stateFan" };
const char MAIN_STATE_HEATPUMP[] PROGMEM            = { "stateHeatpump" };
const char MAIN_STATE_HEAT_ELEMENT[] PROGMEM        = { "stateElement" };
const char MAIN_STATE_EXT_BOILER[] PROGMEM          = { "stateExtBoiler" };
const char MAIN_STATE_DEFROST[] PROGMEM             = { "stateDefrost" };

const char MAIN_SETTING_PWM_01[] PROGMEM              = { "settingPWM_1" };
const char MAIN_SETTING_PWM_02[] PROGMEM              = { "settingPWM_2" };
const char MAIN_SETTING_PWM_03[] PROGMEM              = { "settingPWM_3" };
const char MAIN_SETTING_MIN_TEMP_TARGET[] PROGMEM     = { "settingMinTargetTemp" };
const char MAIN_SETTING_MIN_TEMP_LEGIONELLA[] PROGMEM = { "settingMinLegionellaTemp" };
const char MAIN_SETTING_PWR_HEATELEM[] PROGMEM        = { "settingWattageElement" };
const char MAIN_SETTING_BOILER_CAP[] PROGMEM          = { "settingBoilerCapacity" };
const char MAIN_SETTING_BOILER_BRAND[] PROGMEM        = { "settingBoilerBrand" };
const char MAIN_SETTING_HAS_HEAT_EXC[] PROGMEM        = { "settingHasHeatExchanger" };
const char MAIN_SETTING_HAS_CIRCULATION[] PROGMEM     = { "settingHasCirculation" };
const char MAIN_SETTING_HAS_PV_INPUT[] PROGMEM        = { "settingHasPVInput" };
const char MAIN_SETTING_HAS_EXT_COMM[] PROGMEM        = { "settingHasExtCom" };
const char MAIN_SETTING_HAS_ANTI_TRO[] PROGMEM        = { "settingHasAntiTH" };

const char HMI_HOT_WATER_TEMP_TARGET[] PROGMEM   = { "waterTempTarget" };
const char HMI_OPERATION_MODE[] PROGMEM          = { "operationMode" };
const char HMI_OPERATION_TYPE[] PROGMEM          = { "operationType" };
const char HMI_TIME[] PROGMEM                    = { "time" };
const char HMI_DATE[] PROGMEM                    = { "date" };
const char HMI_TIME_AND_DATE[] PROGMEM           = { "time/date" };
const char HMI_EMERGENCY_MODE[] PROGMEM          = { "emergencyModeEnabled" };
const char HMI_HEATING_ELEMENT_ENABLED[] PROGMEM = { "heatingElementEnabled" };
const char HMI_LEGIONELLA[] PROGMEM              = { "antiLegionellaPerMonth" };
const char HMI_TIMER_WINDOW_A[] PROGMEM          = { "timerWindowA" };
const char HMI_TIMER_WINDOW_B[] PROGMEM          = { "timerWindowB" };
const char HMI_AIR_DUCT_CONFIG[] PROGMEM         = { "configAirduct" };
const char HMI_INSTALLATION_CONFIG[] PROGMEM     = { "configInstallation" };
const char HMI_TEST_MODE[] PROGMEM               = { "testModeStatus" };
const char HMI_SETUP_STATE[] PROGMEM             = { "setupState" };

const char ENERGY_TOTAL_HEATING_ELEM_HOURS[] PROGMEM = { "totalHeatingElemHours" };
const char ENERGY_TOTAL_HEATPUMP_HOURS[] PROGMEM     = { "totalHeatpumpHours" };
const char ENERGY_TOTAL_HOURS[] PROGMEM              = { "totalHours" };
const char ENERGY_TOTAL_ENERGY_WH[] PROGMEM          = { "totalEnergyWh" };
const char ENERGY_POWER_TOTAL[] PROGMEM              = { "powerTotal" };
const char ENERGY_POWER_HEAT_ELEMENT[] PROGMEM       = { "powerHeatingElem" };
const char ENERGY_POWER_HEATPUMP[] PROGMEM           = { "powerHeatpump" };

const char STATS_AQUAMQTT_ADDR[] PROGMEM              = { "ipAddress" };
const char STATS_AQUAMQTT_RSSI[] PROGMEM              = { "rssiDb" };
const char STATS_AQUAMQTT_MODE[] PROGMEM              = { "aquamqttMode" };
const char STATS_AQUAMQTT_OVERRIDE_MODE[] PROGMEM     = { "overrideMode" };
const char STATS_AQUAMQTT_LAST_WILL[] PROGMEM         = { "lwlState" };
const char STATS_MSG_HANDLED[] PROGMEM                = { "msgHandled" };
const char STATS_MSG_UNHANDLED[] PROGMEM              = { "msgUnhandled" };
const char STATS_MSG_SENT[] PROGMEM                   = { "msgSent" };
const char STATS_MSG_CRC_NOK[] PROGMEM                = { "msgCRCNOK" };
const char STATS_DROPPED_BYTES[] PROGMEM              = { "droppedBytes" };
const char STATS_ACTIVE_OVERRIDES[] PROGMEM           = { "activeOverrides" };
const char STATS_ENABLE_FLAG_PV_HEATPUMP[] PROGMEM    = { "flagPVModeHeatPump" };
const char STATS_ENABLE_FLAG_PV_HEATELEMENT[] PROGMEM = { "flagPVModeHeatElement" };

const char DEBUG[] PROGMEM = { "debug" };

// CTRL
const char AQUAMQTT_RESET_OVERRIDES[] PROGMEM = { "reset" };

}  // namespace mqtt
}  // namespace aquamqtt

#endif  // AQUAMQTT_MQTTDEFINITIONS_H
