#ifndef AQUAMQTT_MESSAGECONSTANTS_H
#define AQUAMQTT_MESSAGECONSTANTS_H

#include <cstdio>

#include "mqtt/MQTTDefinitions.h"

namespace aquamqtt::message
{
class IHMIMessage;
constexpr uint8_t HEATPUMP_MAX_FRAME_LENGTH = 70;

constexpr uint8_t MESSAGE_PERIOD_MS = 100;

enum class FrameBufferChannel
{
    CH_LISTENER,
    CH_MAIN,
    CH_HMI
};

enum ProtocolVersion
{
    PROTOCOL_UNKNOWN = -1,
    PROTOCOL_LEGACY  = 0,
    PROTOCOL_NEXT    = 1,
    PROTOCOL_ODYSSEE = 2
};

enum ProtocolChecksum
{
    CHECKSUM_TYPE_UNKNOWN = -1,
    CHECKSUM_TYPE_CRC16   = 0,
    CHECKSUM_TYPE_XOR     = 1,
};

constexpr uint8_t HMI_MESSAGE_IDENTIFIER     = 194;
constexpr uint8_t HMI_MESSAGE_LENGTH_NEXT    = 34;
constexpr uint8_t HMI_MESSAGE_LENGTH_LEGACY  = 35;
constexpr uint8_t HMI_MESSAGE_LENGTH_ODYSSEE = 29;

constexpr uint8_t MAIN_MESSAGE_IDENTIFIER     = 193;
constexpr uint8_t MAIN_MESSAGE_LENGTH_NEXT    = 35;
constexpr uint8_t MAIN_MESSAGE_LENGTH_LEGACY  = 37;
constexpr uint8_t MAIN_MESSAGE_LENGTH_ODYSSEE = 29;

constexpr uint8_t ENERGY_MESSAGE_IDENTIFIER     = 67;
constexpr uint8_t ENERGY_MESSAGE_LENGTH_NEXT    = 45;
constexpr uint8_t ENERGY_MESSAGE_LENGTH_LEGACY  = 31;
constexpr uint8_t ENERGY_MESSAGE_LENGTH_ODYSSEE = 25;

constexpr uint8_t ERROR_MESSAGE_IDENTIFIER     = 74;
constexpr uint8_t ERROR_MESSAGE_LENGTH_NEXT    = 65;
constexpr uint8_t ERROR_MESSAGE_LENGTH_LEGACY  = 35;
constexpr uint8_t ERROR_MESSAGE_LENGTH_ODYSSEE = 68;

constexpr uint8_t EXTRA_MESSAGE_IDENTIFIER     = 217;
constexpr uint8_t EXTRA_MESSAGE_LENGTH_ODYSSEE = 31;

// this will work as long as they will always differ in length
static ProtocolVersion getVersionByIdentifier(uint8_t identifier, uint8_t length)
{
    if (identifier == HMI_MESSAGE_IDENTIFIER)
    {
        switch (length)
        {
            case HMI_MESSAGE_LENGTH_NEXT:
                return PROTOCOL_NEXT;
            case HMI_MESSAGE_LENGTH_ODYSSEE:
                return PROTOCOL_ODYSSEE;
            case HMI_MESSAGE_LENGTH_LEGACY:
                return PROTOCOL_LEGACY;
            default:
                break;
        }
    }
    if (identifier == MAIN_MESSAGE_IDENTIFIER)
    {
        switch (length)
        {
            case MAIN_MESSAGE_LENGTH_NEXT:
                return PROTOCOL_NEXT;
            case MAIN_MESSAGE_LENGTH_ODYSSEE:
                return PROTOCOL_ODYSSEE;
            case MAIN_MESSAGE_LENGTH_LEGACY:
                return PROTOCOL_LEGACY;
            default:
                break;
        }
    }
    if (identifier == ENERGY_MESSAGE_IDENTIFIER)
    {
        switch (length)
        {
            case ENERGY_MESSAGE_LENGTH_NEXT:
                return PROTOCOL_NEXT;
            case ENERGY_MESSAGE_LENGTH_ODYSSEE:
                return PROTOCOL_ODYSSEE;
            case ENERGY_MESSAGE_LENGTH_LEGACY:
                return PROTOCOL_LEGACY;
            default:
                break;
        }
    }
    if (identifier == ERROR_MESSAGE_IDENTIFIER)
    {
        switch (length)
        {
            case ERROR_MESSAGE_LENGTH_NEXT:
                return PROTOCOL_NEXT;
            case ERROR_MESSAGE_LENGTH_ODYSSEE:
                return PROTOCOL_ODYSSEE;
            case ERROR_MESSAGE_LENGTH_LEGACY:
                return PROTOCOL_LEGACY;
            default:
                break;
        }
    }
    if (identifier == EXTRA_MESSAGE_IDENTIFIER)
    {
        switch (length)
        {
            case EXTRA_MESSAGE_LENGTH_ODYSSEE:
                return PROTOCOL_ODYSSEE;
            default:
                break;
        }
    }
    return PROTOCOL_UNKNOWN;
}

static uint8_t lengthByFrameIdAndProtocol(const uint8_t frameId, const ProtocolVersion version)
{
    if (frameId == HMI_MESSAGE_IDENTIFIER)
    {
        switch (version)
        {
            case PROTOCOL_NEXT:
                return HMI_MESSAGE_LENGTH_NEXT;
            case PROTOCOL_ODYSSEE:
                return HMI_MESSAGE_LENGTH_ODYSSEE;
            case PROTOCOL_LEGACY:
            default:
                return HMI_MESSAGE_LENGTH_LEGACY;
        }
    }
    if (frameId == MAIN_MESSAGE_IDENTIFIER)
    {
        switch (version)
        {
            case PROTOCOL_NEXT:
                return MAIN_MESSAGE_LENGTH_NEXT;
            case PROTOCOL_ODYSSEE:
                return MAIN_MESSAGE_LENGTH_ODYSSEE;
            case PROTOCOL_LEGACY:
            default:
                return MAIN_MESSAGE_LENGTH_LEGACY;
        }
    }
    if (frameId == ENERGY_MESSAGE_IDENTIFIER)
    {
        switch (version)
        {
            case PROTOCOL_NEXT:
                return ENERGY_MESSAGE_LENGTH_NEXT;
            case PROTOCOL_ODYSSEE:
                return ENERGY_MESSAGE_LENGTH_ODYSSEE;
            case PROTOCOL_LEGACY:
            default:
                return ENERGY_MESSAGE_LENGTH_LEGACY;
        }
    }
    if (frameId == ERROR_MESSAGE_IDENTIFIER)
    {
        switch (version)
        {
            case PROTOCOL_NEXT:
                return ERROR_MESSAGE_LENGTH_NEXT;
            case PROTOCOL_ODYSSEE:
                return ERROR_MESSAGE_LENGTH_ODYSSEE;
            case PROTOCOL_LEGACY:
            default:
                return ERROR_MESSAGE_LENGTH_LEGACY;
        }
    }
    if (frameId == EXTRA_MESSAGE_IDENTIFIER)
    {
        return EXTRA_MESSAGE_LENGTH_ODYSSEE;
    }
    return 0;
}

static const char* protocolVersionStr(ProtocolVersion version)
{
    switch (version)
    {
        case PROTOCOL_LEGACY:
            return reinterpret_cast<const char*>(mqtt::ENUM_AQUAMQTT_PROTOCOL_LEGACY);
        case PROTOCOL_NEXT:
            return reinterpret_cast<const char*>(mqtt::ENUM_AQUAMQTT_PROTOCOL_NEXT);
        case PROTOCOL_ODYSSEE:
            return reinterpret_cast<const char*>(mqtt::ENUM_AQUAMQTT_PROTOCOL_ODYSSEE);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

static uint8_t generateXorChecksum(const uint8_t* buffer, uint8_t length)
{
    int desiredChecksum = 0;
    // checksum is calculated on length field and payload
    for (size_t i = 0; i < length; i++)
    {
        // XOR the current byte with the accumulated checksum
        desiredChecksum ^= buffer[i];
    }
    return desiredChecksum;
}

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

enum HMIOperationType
{
    OT_UNKNOWN,
    OT_ALWAYS_ON,
    OT_TIMER,
    OT_OFF_PEAK_HOURS
};

static const char* operationTypeStr(const HMIOperationType type)
{
    switch (type)
    {
        case OT_ALWAYS_ON:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_TYPE_ALWAYS_ON);
        case OT_TIMER:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_TYPE_TIMER);
        case OT_OFF_PEAK_HOURS:
            return reinterpret_cast<const char*>(mqtt::ENUM_OPERATION_TYPE_OFF_PEAK_HOURS);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

enum HMIAirDuctConfig : uint8_t
{
    AD_UNKNOWN,
    AD_INT_INT,
    AD_INT_EXT,
    AD_EXT_EXT
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

enum HMITestMode : uint8_t
{
    TM_UNKNOWN,
    TM_OFF,
    TM_ENTERED,
    TM_HEAT_PUMP,
    TM_HEAT_ELEMENT,
    TM_FAN_LOW,
    TM_FAN_HIGH,
    TM_DEFROST,
    TM_HEAT_PUMP_AND_EXT
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

enum HMIFanExhaust : uint8_t
{
    EXHAUST_UNKNOWN,
    EXHAUST_STOP,
    EXHAUST_LOW_SPEED,
    EXHAUST_HIGH_SPEED
};

static const char* exhaustModeStr(HMIFanExhaust mode)
{
    switch (mode)
    {
        case EXHAUST_STOP:
            return reinterpret_cast<const char*>(mqtt::ENUM_CONFIG_EXHAUST_FAN_STOP);
        case EXHAUST_LOW_SPEED:
            return reinterpret_cast<const char*>(mqtt::ENUM_CONFIG_EXHAUST_FAN_LOW_SPEED);
        case EXHAUST_HIGH_SPEED:
            return reinterpret_cast<const char*>(mqtt::ENUM_CONFIG_EXHAUST_FAN_HIGH_SPEED);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

enum HMIInstallation : uint8_t
{
    INST_HP_UNKNOWN,
    INST_HP_ONLY,
    INST_HP_AND_EXT_PRIO_HP,
    INST_HP_AND_EXT_OPT_HP,
    INST_HP_AND_EXT_OPT_EXT,
    INST_HP_AND_EXT_PRIO_EXT,
    INST_HP_AND_SOLAR,
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

enum HMISetup : uint8_t
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

enum MAINBrands : uint8_t
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

}  // namespace aquamqtt::message

#endif  // AQUAMQTT_MESSAGECONSTANTS_H
