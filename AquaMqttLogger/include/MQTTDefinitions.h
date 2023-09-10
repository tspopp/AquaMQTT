#ifndef AQUAMQTT_MQTTDEFINITIONS_H
#define AQUAMQTT_MQTTDEFINITIONS_H

#define DEBUG_AQUAMQTT 0

const char DEBUG_TOPIC[] PROGMEM = { "tortuga/aquawin/dbg/" };
const char BASE_TOPIC[] PROGMEM  = { "tortuga/aquawin/" };

const char ENUM_UNKNOWN[] PROGMEM = { "UNKNOWN" };

const char ENUM_OPERATION_MODE_AUTO[] PROGMEM    = { "AUTO" };
const char ENUM_OPERATION_MODE_ECO_OFF[] PROGMEM = { "MAN ECO OFF" };
const char ENUM_OPERATION_MODE_ECO_ON[] PROGMEM  = { "MAN ECO ON" };
const char ENUM_OPERATION_MODE_BOOST[] PROGMEM   = { "BOOST" };
const char ENUM_OPERATION_MODE_ABSENCE[] PROGMEM = { "ABSENCE" };

const char ENUM_OPERATION_TYPE_ALWAYS_ON[] PROGMEM = { "ALWAYS ON" };
const char ENUM_OPERATION_TYPE_TIMER[] PROGMEM     = { "TIME WINDOW" };

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

const char STATE_FAN[] PROGMEM          = { "stateFan" };
const char STATE_HEATPUMP[] PROGMEM     = { "stateHeatpump" };
const char STATE_HEAT_ELEMENT[] PROGMEM = { "stateElement" };
const char STATE_EXT_BOILER[] PROGMEM   = { "stateExtBoiler" };
const char STATE_DEFROST[] PROGMEM      = { "stateDefrost" };

#endif  // AQUAMQTT_MQTTDEFINITIONS_H
