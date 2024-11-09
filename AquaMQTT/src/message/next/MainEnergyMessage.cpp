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
    // TODO
    return 0;
}
uint32_t MainEnergyMessage::totalHeatingElemHours()
{
    // TODO
    return 0;
}
uint32_t MainEnergyMessage::totalHours()
{
    // TODO
    return 0;
}
uint64_t MainEnergyMessage::totalEnergyCounter()
{
    // TODO
    return 0;
}
uint16_t MainEnergyMessage::powerHeatpump()
{
    // TODO
    return 0;
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
    size_t  numDiffs                           = 0;
    compareBuffers(mData, data, ENERGY_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
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

}
}  // namespace message
}  // namespace aquamqtt