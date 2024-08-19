#include "message/MainEnergyMessage.h"

#include "message/MessageConstants.h"

namespace aquamqtt
{
namespace message
{

uint32_t MainEnergyMessage::totalHeatpumpHours()
{
    return ((uint32_t) mData[14] << 24) | ((uint32_t) mData[13] << 16) | ((uint32_t) mData[12] << 8)
           | (uint32_t) mData[11];
}
uint32_t MainEnergyMessage::totalHeatingElemHours()
{
    return ((uint32_t) mData[18] << 24) | ((uint32_t) mData[17] << 16) | ((uint32_t) mData[16] << 8)
           | (uint32_t) mData[15];
}
uint32_t MainEnergyMessage::totalHours()
{
    return ((uint32_t) mData[22] << 24) | ((uint32_t) mData[21] << 16) | ((uint32_t) mData[20] << 8)
           | (uint32_t) mData[19];
}
uint64_t MainEnergyMessage::totalEnergyCounter()
{
    return ((uint64_t) mData[30] << 56) | ((uint64_t) mData[29] << 48) | ((uint64_t) mData[28] << 40)
           | ((uint64_t) mData[27] << 32) | ((uint64_t) mData[26] << 24) | ((uint64_t) mData[25] << 16)
           | ((uint64_t) mData[24] << 8) | (uint64_t) mData[23];
}
uint16_t MainEnergyMessage::powerHeatpump()
{
    return ((uint16_t) mData[2] << 8) | (uint16_t) mData[1];
}
uint16_t MainEnergyMessage::powerHeatElement()
{
    return ((uint16_t) mData[4] << 8) | (uint16_t) mData[3];
}
uint16_t MainEnergyMessage::powerOverall()
{
    return ((uint16_t) mData[8] << 8) | (uint16_t) mData[7];
}

uint16_t MainEnergyMessage::totalWaterProduction()
{
    return ((uint16_t) mData[10] << 8) | (uint16_t) mData[9];
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

    uint8_t diffIndices[ENERGY_MESSAGE_LENGTH] = { 0 };
    size_t  numDiffs                           = 0;
    compareBuffers(mData, data, ENERGY_MESSAGE_LENGTH, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 1:
            case 2:
                mPowerHeatpumpChanged = true;
                break;
            case 3:
            case 4:
                mPowerHeatelementChanged = true;
                break;
            case 7:
            case 8:
                mPowerOverallChanged = true;
                break;
            case 9:
            case 10:
                mTotalWaterProductionChanged = true;
                break;
            case 11:
            case 12:
            case 13:
            case 14:
                mTotalHeatpumpHoursChanged = true;
                break;
            case 15:
            case 16:
            case 17:
            case 18:
                mTotalHeatElementHoursChanged = true;
                break;
            case 19:
            case 20:
            case 21:
            case 22:
                mTotalHoursChanged = true;
                break;
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
                mTotalEnergyChanged = true;
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

}  // namespace message
}  // namespace aquamqtt