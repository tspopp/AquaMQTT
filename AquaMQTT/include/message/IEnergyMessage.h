#ifndef AQUAMQTT_IENERGYMESSAGE_H
#define AQUAMQTT_IENERGYMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt
{
namespace message
{

class IEnergyMessage
{
public:
    virtual ~IEnergyMessage() = default;

    virtual uint8_t getLength() = 0;

    virtual uint32_t totalHeatpumpHours() = 0;

    virtual uint32_t totalHeatingElemHours() = 0;

    virtual uint32_t totalHours() = 0;

    virtual uint64_t totalEnergyCounter() = 0;

    virtual uint16_t powerHeatpump() = 0;

    virtual uint16_t powerHeatElement() = 0;

    virtual uint16_t powerOverall() = 0;

    virtual uint16_t totalWaterProduction() = 0;

    virtual void compareWith(uint8_t* data) = 0;

    virtual bool totalHeatpumpHoursChanged() const = 0;

    virtual bool totalHeatingElemHoursChanged() const = 0;

    virtual bool totalHoursChanged() const = 0;

    virtual bool totalEnergyCounterChanged() const = 0;

    virtual bool powerHeatpumpChanged() const = 0;

    virtual bool powerHeatElementChanged() const = 0;

    virtual bool powerOverallChanged() const = 0;

    virtual bool totalWaterProductionChanged() const = 0;
};
}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_IENERGYMESSAGE_H
