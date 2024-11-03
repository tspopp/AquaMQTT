#ifndef AQUAMQTT_IENERGYMESSAGE_H
#define AQUAMQTT_IENERGYMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt::message
{

enum class ENERGY_ATTR_U16
{
    POWER_HEATPUMP,
    POWER_HEATELEMENT,
    POWER_TOTAL,
    WATER_TOTAL
};

enum class ENERGY_ATTR_U32
{
    TOTAL_HEATPUMP_HOURS,
    TOTAL_HEATING_ELEMENT_HOURS,
    TOTAL_HOURS,
};

enum class ENERGY_ATTR_U64
{
    TOTAL_ENERGY,
};

class IEnergyMessage
{
public:
    virtual ~IEnergyMessage() = default;

    virtual uint8_t getLength() = 0;

    virtual uint64_t getAttr(ENERGY_ATTR_U64 attr) = 0;
    virtual uint32_t getAttr(ENERGY_ATTR_U32 attr) = 0;
    virtual uint16_t getAttr(ENERGY_ATTR_U16 attr) = 0;

    virtual bool hasAttr(ENERGY_ATTR_U64 attr) const = 0;
    virtual bool hasAttr(ENERGY_ATTR_U32 attr) const = 0;
    virtual bool hasAttr(ENERGY_ATTR_U16 attr) const = 0;

    virtual bool hasChanged(ENERGY_ATTR_U64 attr) const = 0;
    virtual bool hasChanged(ENERGY_ATTR_U32 attr) const = 0;
    virtual bool hasChanged(ENERGY_ATTR_U16 attr) const = 0;
};
}  // namespace aquamqtt::message

#endif  // AQUAMQTT_IENERGYMESSAGE_H
