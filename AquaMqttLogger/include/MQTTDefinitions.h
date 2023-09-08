#ifndef AQUAMQTT_MQTTDEFINITIONS_H
#define AQUAMQTT_MQTTDEFINITIONS_H

const char DEBUG_TOPIC[] PROGMEM = { "tortuga/aquawin/dbg/" };
const char BASE_TOPIC[] PROGMEM  = { "tortuga/aquawin/" };

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
const char TIME[] PROGMEM                      = { "time" };
const char DATE[] PROGMEM                      = { "date" };
const char POWER_TOTAL[] PROGMEM               = { "powerTotal" };
const char POWER_HEAT_ELEMENT[] PROGMEM        = { "powerHeatingElem" };
const char POWER_HEATPUMP[] PROGMEM            = { "powerHeatpump" };
const char FAN_SPEED[] PROGMEM                 = { "fanSpeed" };
const char UNKNOWN_67A[] PROGMEM               = { "67a" };
const char BITMASK_193[] PROGMEM               = { "193bitmask" };

#endif  // AQUAMQTT_MQTTDEFINITIONS_H
