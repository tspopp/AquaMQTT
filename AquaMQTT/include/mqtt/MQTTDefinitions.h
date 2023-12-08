#ifndef AQUAMQTT_MQTTDEFINITIONS_H
#define AQUAMQTT_MQTTDEFINITIONS_H

namespace aquamqtt
{
namespace mqtt
{
const char BASE_TOPIC[] PROGMEM     = { "aquamqtt/" };
const char CONTROL_TOPIC[] PROGMEM  = { "aquamqtt/ctrl/#" };
const char STATS_SUBTOPIC[] PROGMEM = { "stats/" };

const char HMI_SUBTOPIC[] PROGMEM    = { "hmi/" };
const char MAIN_SUBTOPIC[] PROGMEM   = { "main/" };
const char ENERGY_SUBTOPIC[] PROGMEM = { "energy/" };

const char ENUM_UNKNOWN[] PROGMEM = { "UNKNOWN" };

const char ENUM_AQUAMQTT_MODE_LISTENER[] PROGMEM = { "listener" };
const char ENUM_AQUAMQTT_MODE_MITM[] PROGMEM     = { "mitm" };

const char ENUM_OPERATION_MODE_AUTO[] PROGMEM    = { "AUTO" };
const char ENUM_OPERATION_MODE_ECO_OFF[] PROGMEM = { "MAN ECO OFF" };
const char ENUM_OPERATION_MODE_ECO_ON[] PROGMEM  = { "MAN ECO ON" };
const char ENUM_OPERATION_MODE_BOOST[] PROGMEM   = { "BOOST" };
const char ENUM_OPERATION_MODE_ABSENCE[] PROGMEM = { "ABSENCE" };

const char ENUM_AIR_DUCT_IN_IN[] PROGMEM   = { "INTERIOR/INTERIOR" };
const char ENUM_AIR_DUCT_EXT_EXT[] PROGMEM = { "EXTERIOR/EXTERIOR" };
const char ENUM_AIR_DUCT_INT_EXT[] PROGMEM = { "INTERIOR/EXTERIOR" };

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

// TODO: distinguish between messages
const char HOT_WATER_TEMP[] PROGMEM            = { "waterTemp" };
const char SUPPLY_AIR_TEMP[] PROGMEM           = { "supplyAirTemp" };
const char EVAPORATOR_AIR_TEMP_UPPER[] PROGMEM = { "evaporatorAirTemp" };
const char EVAPORATOR_AIR_TEMP_LOWER[] PROGMEM = { "evaporatorAirTempLower" };
const char HOT_WATER_TEMP_TARGET[] PROGMEM     = { "waterTempTarget" };
const char TOTAL_HEATING_ELEM_HOURS[] PROGMEM  = { "totalHeatingElemHours" };
const char TOTAL_HEATPUMP_HOURS[] PROGMEM      = { "totalHeatpumpHours" };
const char TOTAL_HOURS[] PROGMEM               = { "totalHours" };
const char TOTAL_ENERGY_WH[] PROGMEM           = { "totalEnergyWh" };
const char OPERATION_MODE[] PROGMEM            = { "operationMode" };
const char OPERATION_TYPE[] PROGMEM            = { "operationType" };
const char TIME[] PROGMEM                      = { "time" };
const char DATE[] PROGMEM                      = { "date" };
const char POWER_TOTAL[] PROGMEM               = { "powerTotal" };
const char POWER_HEAT_ELEMENT[] PROGMEM        = { "powerHeatingElem" };
const char POWER_HEATPUMP[] PROGMEM            = { "powerHeatpump" };
const char FAN_SPEED[] PROGMEM                 = { "fanSpeed" };
const char EMERGENCY_MODE[] PROGMEM            = { "emergencyMode" };
const char HEATING_ELEMENT_ENABLED[] PROGMEM   = { "heatingElementEnabled" };
const char LEGIONELLA[] PROGMEM                = { "antiLegionellaPerMonth" };
const char TIMER_WINDOW_A[] PROGMEM            = { "timerWindowA" };
const char TIMER_WINDOW_B[] PROGMEM            = { "timerWindowB" };

const char AIR_DUCT_CONFIG[] PROGMEM     = { "configAirduct" };
const char INSTALLATION_CONFIG[] PROGMEM = { "configInstallation" };
const char TEST_MODE[] PROGMEM           = { "testModeStatus" };

const char STATE_FAN[] PROGMEM          = { "stateFan" };
const char STATE_HEATPUMP[] PROGMEM     = { "stateHeatpump" };
const char STATE_HEAT_ELEMENT[] PROGMEM = { "stateElement" };
const char STATE_EXT_BOILER[] PROGMEM   = { "stateExtBoiler" };
const char STATE_DEFROST[] PROGMEM      = { "stateDefrost" };

const char AQUAMQTT_ADDR[] PROGMEM      = { "ipAddress" };
const char AQUAMQTT_RSSI[] PROGMEM      = { "rssiDb" };
const char AQUAMQTT_MODE[] PROGMEM      = { "aquamqttMode" };
const char AQUAMQTT_LAST_WILL[] PROGMEM = { "lwlState" };

const char STATS_MSG_HANDLED[] PROGMEM   = { "msgHandled" };
const char STATS_MSG_UNHANDLED[] PROGMEM = { "msgUnhandled" };
const char STATS_MSG_SENT[] PROGMEM      = { "msgSent" };
const char STATS_MSG_CRC_NOK[] PROGMEM   = { "msgCRCNOK" };
const char STATS_DROPPED_BYTES[] PROGMEM = { "droppedBytes" };

const char AQUAMQTT_LAST_WILL_ONLINE[] PROGMEM  = { "online" };
const char AQUAMQTT_LAST_WILL_OFFLINE[] PROGMEM = { "offline" };



}  // namespace mqtt
}  // namespace aquamqtt

#endif  // AQUAMQTT_MQTTDEFINITIONS_H
