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

    uint32_t totalHeatpumpHours()
    {
        return ((uint32_t) mData[14] << 24) | ((uint32_t) mData[13] << 16) | ((uint32_t) mData[12] << 8)
               | (uint32_t) mData[11];
    }

    uint32_t totalHeatingElemHours()
    {
        return ((uint32_t) mData[18] << 24) | ((uint32_t) mData[17] << 16) | ((uint32_t) mData[16] << 8)
               | (uint32_t) mData[15];
    }

    uint32_t totalHours()
    {
        return ((uint32_t) mData[22] << 24) | ((uint32_t) mData[21] << 16) | ((uint32_t) mData[20] << 8)
               | (uint32_t) mData[19];
    }

    uint64_t totalEnergyCounter()
    {
        return ((uint64_t) mData[30] << 56) | ((uint64_t) mData[29] << 48) | ((uint64_t) mData[28] << 40)
               | ((uint64_t) mData[27] << 32) | ((uint64_t) mData[26] << 24) | ((uint64_t) mData[25] << 16)
               | ((uint64_t) mData[24] << 8) | (uint64_t) mData[23];
    }

    uint16_t powerHeatpump()
    {
        return ((uint16_t) mData[2] << 8) | (uint16_t) mData[1];
    }

    uint16_t powerHeatElement()
    {
        return ((uint16_t) mData[4] << 8) | (uint16_t) mData[3];
    }

    uint16_t powerOverall()
    {
        return ((uint16_t) mData[8] << 8) | (uint16_t) mData[7];
    }

private:
    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MAINENERGYMESSAGE_H
