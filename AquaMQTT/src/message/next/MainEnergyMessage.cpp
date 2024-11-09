#include "message/next/MainEnergyMessage.h"

#include "message/MessageConstants.h"

namespace aquamqtt
{
namespace message
{
namespace next
{

uint32_t MainEnergyMessage::totalHeatpumpHours()
{
    return ((uint32_t) mData[16] << 24) | ((uint32_t) mData[15] << 16) | ((uint32_t) mData[14] << 8)
           | (uint32_t) mData[13];
}
uint32_t MainEnergyMessage::totalHeatingElemHours()
{
    // TODO
    return 0;
}
uint32_t MainEnergyMessage::totalHours()
{
    return ((uint32_t) mData[6] << 24) | ((uint32_t) mData[5] << 16) | ((uint32_t) mData[4] << 8)
           | (uint32_t) mData[3];
}
uint64_t MainEnergyMessage::totalEnergyCounter()
{
    // TODO
    return 0;
}
uint16_t MainEnergyMessage::powerHeatpump()
{
    return ((uint16_t) mData[22] << 8) | (uint16_t) mData[21];
}
uint16_t MainEnergyMessage::powerHeatElement()
{
    // TODO
    return 0;
}
uint16_t MainEnergyMessage::powerOverall()
{
    // TODO
    return 0;
}

uint16_t MainEnergyMessage::totalWaterProduction()
{
    // TODO
    return 0;
}

void MainEnergyMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        mPowerHeatpumpChanged         = true;
        mPowerHeatelementChanged      = true;
        mPowerOverallChanged          = true;
        mTotalHeatpumpHoursChanged    = true;
        mTotalHeatElementHoursChanged = true;
        mTotalHoursChanged            = true;
        mTotalEnergyChanged           = true;
        mTotalWaterProductionChanged  = true;
        return;
    }

    uint8_t diffIndices[ENERGY_MESSAGE_LENGTH_NEXT] = { 0 };
    size_t  numDiffs                                = 0;
    compareBuffers(mData, data, ENERGY_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 3:
            case 4:
            case 5:
            case 6:
                mTotalHoursChanged = true;
                break;
            case 13:
            case 14:
            case 15:
            case 16:
                mTotalHeatpumpHoursChanged = true;
                break;
            case 21:
            case 22:
                mPowerHeatpumpChanged = true;
                break;
            default:
                break;
        }
    }
}
MainEnergyMessage::MainEnergyMessage(uint8_t* data)
    : mData(data)
    , mPowerHeatpumpChanged(false)
    , mPowerHeatelementChanged(false)
    , mPowerOverallChanged(false)
    , mTotalHeatpumpHoursChanged(false)
    , mTotalHeatElementHoursChanged(false)
    , mTotalHoursChanged(false)
    , mTotalEnergyChanged(false)
    , mTotalWaterProductionChanged(false)
{
}
bool MainEnergyMessage::totalHeatpumpHoursChanged() const
{
    return mTotalHeatpumpHoursChanged;
}
bool MainEnergyMessage::totalHeatingElemHoursChanged() const
{
    return mTotalHeatElementHoursChanged;
}
bool MainEnergyMessage::totalHoursChanged() const
{
    return mTotalHoursChanged;
}
bool MainEnergyMessage::totalEnergyCounterChanged() const
{
    return mTotalEnergyChanged;
}
bool MainEnergyMessage::powerHeatpumpChanged() const
{
    return mPowerHeatpumpChanged;
}
bool MainEnergyMessage::powerHeatElementChanged() const
{
    return mPowerHeatelementChanged;
}
bool MainEnergyMessage::powerOverallChanged() const
{
    return mPowerOverallChanged;
}
bool MainEnergyMessage::totalWaterProductionChanged() const
{
    return mTotalWaterProductionChanged;
}
uint8_t MainEnergyMessage::getLength()
{
    return ENERGY_MESSAGE_LENGTH_NEXT;
}

}  // namespace next
}  // namespace message
}  // namespace aquamqtt