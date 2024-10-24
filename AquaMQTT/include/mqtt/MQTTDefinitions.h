#ifndef AQUAMQTT_MQTTDEFINITIONS_H
#define AQUAMQTT_MQTTDEFINITIONS_H

namespace aquamqtt
{
namespace mqtt
{
// Base Topics
constexpr char BASE_TOPIC[]    = { "aquamqtt/" };
constexpr char CONTROL_TOPIC[] = { "aquamqtt/ctrl/#" };

constexpr char HMI_SUBTOPIC[]    = { "hmi/" };
constexpr char MAIN_SUBTOPIC[]   = { "main/" };
constexpr char ENERGY_SUBTOPIC[] = { "energy/" };
constexpr char ERROR_SUBTOPIC[]  = { "error/" };
constexpr char STATS_SUBTOPIC[]  = { "stats/" };

// Enum Types
constexpr char ENUM_UNKNOWN[] = { "UNKNOWN" };

constexpr char ENUM_AQUAMQTT_MODE_LISTENER[] = { "LISTENER" };
constexpr char ENUM_AQUAMQTT_MODE_MITM[]     = { "MITM" };

constexpr char ENUM_BRAND_ATLANTIC[] = { "Atlantic" };
constexpr char ENUM_BRAND_THERMOR[]  = { "Thermor" };
constexpr char ENUM_BRAND_SAUTER[]   = { "Sauter" };
constexpr char ENUM_BRAND_NONAME[]   = { "No Name" };

constexpr char ENUM_OPERATION_MODE_AUTO[]    = { "AUTO" };
constexpr char ENUM_OPERATION_MODE_ECO_OFF[] = { "MAN ECO OFF" };
constexpr char ENUM_OPERATION_MODE_ECO_ON[]  = { "MAN ECO ON" };
constexpr char ENUM_OPERATION_MODE_BOOST[]   = { "BOOST" };
constexpr char ENUM_OPERATION_MODE_ABSENCE[] = { "ABSENCE" };

constexpr char ENUM_AIR_DUCT_IN_IN[]   = { "INT/INT" };
constexpr char ENUM_AIR_DUCT_EXT_EXT[] = { "EXT/EXT" };
constexpr char ENUM_AIR_DUCT_INT_EXT[] = { "INT/EXT" };

constexpr char ENUM_INSTALLATION_THERMODYNAMICS_ONLY[]    = { "HEAT PUMP ONLY" };
constexpr char ENUM_INSTALLATION_BOILER_BACKUP_HP_PRIO[]  = { "BOILER BACKUP / HEAT PUMP PRIORITY" };
constexpr char ENUM_INSTALLATION_BOILER_BACKUP_HP_OPT[]   = { "BOILER BACKUP / HEAT PUMP OPTIMIZED" };
constexpr char ENUM_INSTALLATION_BOILER_BACKUP_EXT_OPT[]  = { "BOILER BACKUP / BOILER OPTIMIZED" };
constexpr char ENUM_INSTALLATION_BOILER_BACKUP_EXT_PRIO[] = { "BOILER BACKUP / BOILER PRIORITY" };
constexpr char ENUM_INSTALLATION_SOLAR_BACKUP[]           = { "HEAT PUMP AND SOLAR BACKUP" };

constexpr char ENUM_CONFIG_EXHAUST_FAN_STOP[]       = { "STOP" };
constexpr char ENUM_CONFIG_EXHAUST_FAN_LOW_SPEED[]  = { "LOW SPEED" };
constexpr char ENUM_CONFIG_EXHAUST_FAN_HIGH_SPEED[] = { "HIGH SPEED" };

constexpr char ENUM_TEST_MODE_OFF[]                            = { "OFF" };
constexpr char ENUM_TEST_MODE_IDLE[]                           = { "IDLE" };
constexpr char ENUM_TEST_MODE_ACTIVE_HEAT_PUMP[]               = { "TESTING HEAT PUMP" };
constexpr char ENUM_TEST_MODE_ACTIVE_HEAT_ELEM[]               = { "TESTING HEAT ELEM" };
constexpr char ENUM_TEST_MODE_ACTIVE_FAN_HIGH[]                = { "TESTING FAN HIGH" };
constexpr char ENUM_TEST_MODE_ACTIVE_FAN_LOW[]                 = { "TESTING FAN LOW" };
constexpr char ENUM_TEST_MODE_ACTIVE_DEFROSTING[]              = { "TESTING DEFROSTING" };
constexpr char ENUM_TEST_MODE_ACTIVE_HEAT_PUMP_BOILER_BACKUP[] = { "TESTING HEAT PUMP AND BOILER BACKUP" };

constexpr char ENUM_OPERATION_TYPE_ALWAYS_ON[] = { "ALWAYS ON" };
constexpr char ENUM_OPERATION_TYPE_TIMER[]     = { "TIME WINDOW" };

constexpr char ENUM_LAST_WILL_ONLINE[]  = { "ONLINE" };
constexpr char ENUM_LAST_WILL_OFFLINE[] = { "OFFLINE" };

constexpr char ENUM_SETUP_COMPLETED[]  = { "OK" };
constexpr char ENUM_SETUP_INCOMPLETE[] = { "PARTIAL RESET" };
constexpr char ENUM_SETUP_RESET[]      = { "FACTORY RESET" };

constexpr char ENUM_AQUAMQTT_OVERRIDE_MODE_DEFAULT[] = { "DEFAULT" };
constexpr char ENUM_AQUAMQTT_OVERRIDE_MODE_HP_ONLY[] = { "PV HP" };
constexpr char ENUM_AQUAMQTT_OVERRIDE_MODE_HE_ONLY[] = { "PV HE" };
constexpr char ENUM_AQUAMQTT_OVERRIDE_MODE_PV_FULL[] = { "PV BOOST" };

// Subtopics
constexpr char MAIN_ERROR_CODE[]                = { "errorCode" };
constexpr char MAIN_HOT_WATER_TEMP[]            = { "waterTemp" };
constexpr char MAIN_SUPPLY_AIR_TEMP[]           = { "supplyAirTemp" };
constexpr char MAIN_EVAPORATOR_AIR_TEMP_UPPER[] = { "evaporatorAirTemp" };
constexpr char MAIN_EVAPORATOR_AIR_TEMP_LOWER[] = { "evaporatorAirTempLower" };
constexpr char MAIN_FAN_PWM[]                   = { "fanPWM" };
constexpr char MAIN_STATE_FAN[]                 = { "stateFan" };
constexpr char MAIN_STATE_HEATPUMP[]            = { "stateHeatpump" };
constexpr char MAIN_STATE_HEAT_ELEMENT[]        = { "stateElement" };
constexpr char MAIN_STATE_EXT_BOILER[]          = { "stateExtBoiler" };
constexpr char MAIN_STATE_DEFROST[]             = { "stateDefrost" };
constexpr char MAIN_STATE_PV[]                  = { "statePV" };
constexpr char MAIN_STATE_SOLAR[]               = { "stateSolar" };

constexpr char MAIN_SETTING_PWM_01[]              = { "settingPWM_1" };
constexpr char MAIN_SETTING_PWM_02[]              = { "settingPWM_2" };
constexpr char MAIN_SETTING_PWM_03[]              = { "settingPWM_3" };
constexpr char MAIN_SETTING_MIN_TEMP_TARGET[]     = { "settingMinTargetTemp" };
constexpr char MAIN_SETTING_MIN_TEMP_LEGIONELLA[] = { "settingMinLegionellaTemp" };
constexpr char MAIN_SETTING_PWR_HEATELEM[]        = { "settingWattageElement" };
constexpr char MAIN_SETTING_BOILER_CAP[]          = { "settingBoilerCapacity" };
constexpr char MAIN_SETTING_BOILER_BRAND[]        = { "settingBoilerBrand" };
constexpr char MAIN_CAPABILITY_HEAT_EXC[]         = { "capabilityHeatExchanger" };
constexpr char MAIN_CAPABILITY_CIRCULATION[]      = { "capabilityCirculation" };
constexpr char MAIN_CAPABILITY_PV_INPUT[]         = { "capabilityPVInput" };
constexpr char MAIN_CAPABILITY_EXT_COMM[]         = { "capabilityCommunication" };
constexpr char MAIN_CAPABILITY_DRY_HEATING[]      = { "capabilityAntiDryHeating" };

constexpr char HMI_HOT_WATER_TEMP_TARGET[]   = { "waterTempTarget" };
constexpr char HMI_OPERATION_MODE[]          = { "operationMode" };
constexpr char HMI_OPERATION_TYPE[]          = { "operationType" };
constexpr char HMI_TIME[]                    = { "time" };
constexpr char HMI_DATE[]                    = { "date" };
constexpr char HMI_TIME_AND_DATE[]           = { "time/date" };
constexpr char HMI_EMERGENCY_MODE[]          = { "emergencyModeEnabled" };
constexpr char HMI_HEATING_ELEMENT_ENABLED[] = { "heatingElementEnabled" };
constexpr char HMI_LEGIONELLA[]              = { "antiLegionellaPerMonth" };
constexpr char HMI_TIMER_WINDOW_A[]          = { "timerWindowA" };
constexpr char HMI_TIMER_WINDOW_B[]          = { "timerWindowB" };
constexpr char HMI_AIR_DUCT_CONFIG[]         = { "configAirduct" };
constexpr char HMI_FAN_EXHAUST_CONFIG[]      = { "configFanExhaust" };
constexpr char HMI_INSTALLATION_CONFIG[]     = { "configInstallation" };
constexpr char HMI_TEST_MODE[]               = { "testModeStatus" };
constexpr char HMI_SETUP_STATE[]             = { "setupState" };
constexpr char HMM_PV_INPUT_ACTIVATED[]      = { "pvInputActivated" };

constexpr char ENERGY_TOTAL_HEATING_ELEM_HOURS[] = { "totalHeatingElemHours" };
constexpr char ENERGY_TOTAL_HEATPUMP_HOURS[]     = { "totalHeatpumpHours" };
constexpr char ENERGY_TOTAL_HOURS[]              = { "totalHours" };
constexpr char ENERGY_TOTAL_ENERGY_WH[]          = { "totalEnergyWh" };
constexpr char ENERGY_POWER_TOTAL[]              = { "powerTotal" };
constexpr char ENERGY_POWER_HEAT_ELEMENT[]       = { "powerHeatingElem" };
constexpr char ENERGY_POWER_HEATPUMP[]           = { "powerHeatpump" };
constexpr char ENERGY_TOTAL_WATER_PRODUCTION[]   = { "totalWaterProduction" };

constexpr char ERROR_ERROR_NUMBER[] = { "errorNumber" };

constexpr char STATS_AQUAMQTT_ADDR[]              = { "ipAddress" };
constexpr char STATS_AQUAMQTT_RSSI[]              = { "rssiDb" };
constexpr char STATS_AQUAMQTT_MODE[]              = { "aquamqttMode" };
constexpr char STATS_AQUAMQTT_OVERRIDE_MODE[]     = { "overrideMode" };
constexpr char STATS_AQUAMQTT_LAST_WILL[]         = { "lwlState" };
constexpr char STATS_MSG_HANDLED[]                = { "msgHandled" };
constexpr char STATS_MSG_UNHANDLED[]              = { "msgUnhandled" };
constexpr char STATS_MSG_SENT[]                   = { "msgSent" };
constexpr char STATS_MSG_CRC_NOK[]                = { "msgCRCNOK" };
constexpr char STATS_DROPPED_BYTES[]              = { "droppedBytes" };
constexpr char STATS_ACTIVE_OVERRIDES[]           = { "activeOverrides" };
constexpr char STATS_ACTIVE_OVERRIDES_MAIN[]      = { "activeOverridesMain" };
constexpr char STATS_ENABLE_FLAG_PV_HEATPUMP[]    = { "flagPVModeHeatPump" };
constexpr char STATS_ENABLE_FLAG_PV_HEATELEMENT[] = { "flagPVModeHeatElement" };

constexpr char DEBUG[] = { "debug" };

// CTRL
constexpr char AQUAMQTT_RESET_OVERRIDES[] = { "reset" };

}  // namespace mqtt
}  // namespace aquamqtt

#endif  // AQUAMQTT_MQTTDEFINITIONS_H
