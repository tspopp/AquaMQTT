#ifndef AQUAMQTT_MAINENERGYMESSAGE_H
#define AQUAMQTT_MAINENERGYMESSAGE_H

#include <cstring>

namespace aquamqtt
{
namespace message
{
class MainEnergyMessage
{
public:
    explicit MainEnergyMessage(uint8_t* data) : mData(data)
    {
    }

    ~MainEnergyMessage() = default;

    uint32_t totalHeatpumpHours();

    uint32_t totalHeatingElemHours();

    uint32_t totalHours();

    uint64_t totalEnergyCounter();

    uint16_t powerHeatpump();

    uint16_t powerHeatElement();

    uint16_t powerOverall();

private:
    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MAINENERGYMESSAGE_H
