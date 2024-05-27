#ifndef AQUAMQTT_MESSAGECONSTANTS_H
#define AQUAMQTT_MESSAGECONSTANTS_H

#include <cstdio>

#include "mqtt/MQTTDefinitions.h"

namespace aquamqtt
{
namespace message
{
constexpr uint8_t HEATPUMP_MAX_FRAME_LENGTH = 40;

constexpr uint8_t HMI_MESSAGE_IDENTIFIER    = 194;
constexpr uint8_t HMI_MESSAGE_LENGTH        = 35;
constexpr uint8_t MAIN_MESSAGE_IDENTIFIER   = 193;
constexpr uint8_t MAIN_MESSAGE_LENGTH       = 37;
constexpr uint8_t ENERGY_MESSAGE_IDENTIFIER = 67;
constexpr uint8_t ENERGY_MESSAGE_LENGTH     = 31;
constexpr uint8_t ERROR_MESSAGE_IDENTIFIER  = 74;
constexpr uint8_t ERROR_MESSAGE_LENGTH      = 35;

enum HMIOperationMode : int
{
    OM_UNKNOWN      = -1,
    OM_ABSENCE      = 0,
    OM_ECO_ACTIVE   = 1,
    OM_ECO_INACTIVE = 2,
    OM_BOOST        = 3,
    OM_AUTO         = 4
};

static const char* operationModeStr(HMIOperationMode mode)
{
    switch (mode)
    {
        case OM_ABSENCE:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_MODE_ABSENCE);
        case OM_ECO_ACTIVE:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_MODE_ECO_ON);
        case OM_ECO_INACTIVE:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_MODE_ECO_OFF);
        case OM_BOOST:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_MODE_BOOST);
        case OM_AUTO:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_MODE_AUTO);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

enum HMIOperationType : bool
{
    ALWAYS_ON = false,
    TIMER     = true
};

static const char* operationTypeStr(HMIOperationType type)
{
    switch (type)
    {
        case ALWAYS_ON:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_TYPE_ALWAYS_ON);
        case TIMER:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_TYPE_TIMER);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

enum HMIAirDuctConfig : int
{
    AD_UNKNOWN = -1,
    AD_INT_INT = 0,   // Umluft
    AD_INT_EXT = 16,  // 1 Kanal Luftanschluss
    AD_EXT_EXT = 32   // 2 Kanal Luftanschluss
};

static const char* airDuctConfigStr(HMIAirDuctConfig config)
{
    switch (config)
    {
        case AD_INT_INT:
            return reinterpret_cast<const char*>(mqtt::ENUM_AIR_DUCT_IN_IN);
        case AD_INT_EXT:
            return reinterpret_cast<const char*>(mqtt::ENUM_AIR_DUCT_INT_EXT);
        case AD_EXT_EXT:
            return reinterpret_cast<const char*>(mqtt::ENUM_AIR_DUCT_EXT_EXT);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

enum HMITestMode : int
{
    TM_UNKNOWN           = -1,
    TM_OFF               = 0,
    TM_ENTERED           = 1,
    TM_HEAT_PUMP         = 2,
    TM_HEAT_ELEMENT      = 3,
    TM_FAN_LOW           = 4,
    TM_FAN_HIGH          = 5,
    TM_DEFROST           = 6,
    TM_HEAT_PUMP_AND_EXT = 8
};

static const char* testModeStr(HMITestMode config)
{
    switch (config)
    {
        case TM_OFF:
            return reinterpret_cast<const char*>(mqtt::ENUM_TEST_MODE_OFF);
        case TM_ENTERED:
            return reinterpret_cast<const char*>(mqtt::ENUM_TEST_MODE_IDLE);
        case TM_HEAT_PUMP:
            return reinterpret_cast<const char*>(mqtt::ENUM_TEST_MODE_ACTIVE_HEAT_PUMP);
        case TM_HEAT_ELEMENT:
            return reinterpret_cast<const char*>(mqtt::ENUM_TEST_MODE_ACTIVE_HEAT_ELEM);
        case TM_FAN_LOW:
            return reinterpret_cast<const char*>(mqtt::ENUM_TEST_MODE_ACTIVE_FAN_LOW);
        case TM_FAN_HIGH:
            return reinterpret_cast<const char*>(mqtt::ENUM_TEST_MODE_ACTIVE_FAN_HIGH);
        case TM_DEFROST:
            return reinterpret_cast<const char*>(mqtt::ENUM_TEST_MODE_ACTIVE_DEFROSTING);
        case TM_HEAT_PUMP_AND_EXT:
            return reinterpret_cast<const char*>(mqtt::ENUM_TEST_MODE_ACTIVE_HEAT_PUMP_BOILER_BACKUP);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

enum HMIInstallation
{
    INST_HP_UNKNOWN          = -1,
    INST_HP_ONLY             = 0,   // 0000 0000
    INST_HP_AND_EXT_PRIO_HP  = 1,   // 0000 0001
    INST_HP_AND_EXT_OPT_HP   = 17,  // 0001 0001
    INST_HP_AND_EXT_OPT_EXT  = 33,  // 0010 0001
    INST_HP_AND_EXT_PRIO_EXT = 49,  // 0011 0001
    INST_HP_AND_SOLAR        = 2,   // 0000 0010
};

static const char* installationModeStr(HMIInstallation installation)
{
    switch (installation)
    {
        case INST_HP_ONLY:
            return reinterpret_cast<const char*>(mqtt::ENUM_INSTALLATION_THERMODYNAMICS_ONLY);
        case INST_HP_AND_EXT_PRIO_HP:
            return reinterpret_cast<const char*>(mqtt::ENUM_INSTALLATION_BOILER_BACKUP_HP_PRIO);
        case INST_HP_AND_EXT_OPT_HP:
            return reinterpret_cast<const char*>(mqtt::ENUM_INSTALLATION_BOILER_BACKUP_HP_OPT);
        case INST_HP_AND_EXT_OPT_EXT:
            return reinterpret_cast<const char*>(mqtt::ENUM_INSTALLATION_BOILER_BACKUP_EXT_OPT);
        case INST_HP_AND_EXT_PRIO_EXT:
            return reinterpret_cast<const char*>(mqtt::ENUM_INSTALLATION_BOILER_BACKUP_EXT_PRIO);
        case INST_HP_AND_SOLAR:
            return reinterpret_cast<const char*>(mqtt::ENUM_INSTALLATION_SOLAR_BACKUP);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

enum HMISetup
{
    COMPLETED,
    INCOMPLETE,
    RESET
};

static const char* setupStr(HMISetup installation)
{
    switch (installation)
    {
        case INCOMPLETE:
            return reinterpret_cast<const char*>(mqtt::ENUM_SETUP_INCOMPLETE);
        case RESET:
            return reinterpret_cast<const char*>(mqtt::ENUM_SETUP_RESET);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_SETUP_COMPLETED);
    }
}

static void toHexStr(uint8_t* data, uint8_t data_len, char* buffer)
{

    const size_t num_bytes = data_len / sizeof(uint8_t);
    // char         hex_str[num_bytes * 2 + 1];
    for (size_t i = 0; i < num_bytes; i++)
    {
        sprintf(&buffer[i * 2], "%02X", data[i]);
    }
    buffer[num_bytes * 2] = '\0';
}

enum MAINBrands
{
    BR_ATLANTIC,
    BR_NONAME,
    BR_SAUTER,
    BR_THERMOR,
    BR_UNKNOWN
};

static const char* brandStr(MAINBrands brand)
{
    switch (brand)
    {
        case BR_ATLANTIC:
            return reinterpret_cast<const char*>(mqtt::ENUM_BRAND_ATLANTIC);
        case BR_NONAME:
            return reinterpret_cast<const char*>(mqtt::ENUM_BRAND_NONAME);
        case BR_SAUTER:
            return reinterpret_cast<const char*>(mqtt::ENUM_BRAND_SAUTER);
        case BR_THERMOR:
            return reinterpret_cast<const char*>(mqtt::ENUM_BRAND_THERMOR);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

void static compareBuffers(
        const uint8_t* buffer1,
        const uint8_t* buffer2,
        size_t         length,
        uint8_t*       diffIndices,
        size_t*        numDiffs)
{
    *numDiffs = 0;
    for (size_t i = 0; i < length; i++)
    {
        if (buffer1[i] != buffer2[i])
        {
            diffIndices[(*numDiffs)++] = i;
        }
    }
}

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MESSAGECONSTANTS_H
