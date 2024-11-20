#include "message/next/MainEnergyMessage.h"

#include "message/MessageConstants.h"

namespace aquamqtt::message::next
{

MainEnergyMessage::MainEnergyMessage(uint8_t* data, uint8_t* previous)
    : mData(data)
    , mHasChangedU16()
    , mHasChangedU32()
    , mHasChangedU64()
{
    mCreatedWithoutPrevious = previous == nullptr;
    compareWith(previous);
}

void MainEnergyMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
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
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HOURS);
                break;
            case 13:
            case 14:
            case 15:
            case 16:
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS);
                break;
            case 21:
            case 22:
                mHasChangedU16.insert(ENERGY_ATTR_U16::POWER_HEATPUMP);
                break;
            default:
                break;
        }
    }
}

uint8_t MainEnergyMessage::getLength()
{
    return ENERGY_MESSAGE_LENGTH_NEXT;
}

uint64_t MainEnergyMessage::getAttr(ENERGY_ATTR_U64 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U64::TOTAL_ENERGY:
            break;
    }
    return 0;
}

uint32_t MainEnergyMessage::getAttr(ENERGY_ATTR_U32 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS:
            return ((uint32_t) mData[16] << 24) | ((uint32_t) mData[15] << 16) | ((uint32_t) mData[14] << 8)
                   | (uint32_t) mData[13];
        case ENERGY_ATTR_U32::TOTAL_HOURS:
            return ((uint32_t) mData[6] << 24) | ((uint32_t) mData[5] << 16) | ((uint32_t) mData[4] << 8)
                   | (uint32_t) mData[3];
        case ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS:
            break;
    }
    return 0;
}

uint16_t MainEnergyMessage::getAttr(ENERGY_ATTR_U16 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U16::POWER_HEATPUMP:
            return ((uint16_t) mData[22] << 8) | (uint16_t) mData[21];
        case ENERGY_ATTR_U16::POWER_HEATELEMENT:
        case ENERGY_ATTR_U16::POWER_TOTAL:
        case ENERGY_ATTR_U16::WATER_TOTAL:
            break;
    }
    return 0;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U64 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U64::TOTAL_ENERGY:
            break;
    }
    return false;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U32 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS:
        case ENERGY_ATTR_U32::TOTAL_HOURS:
            return true;
        case ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS:
            break;
    }
    return false;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U16 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U16::POWER_HEATPUMP:
            return true;
        case ENERGY_ATTR_U16::POWER_HEATELEMENT:
        case ENERGY_ATTR_U16::POWER_TOTAL:
        case ENERGY_ATTR_U16::WATER_TOTAL:
            break;
    }
    return false;
}

bool MainEnergyMessage::hasChanged(ENERGY_ATTR_U64 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU64.find(attr) != mHasChangedU64.end();
}

bool MainEnergyMessage::hasChanged(ENERGY_ATTR_U32 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU32.find(attr) != mHasChangedU32.end();
}

bool MainEnergyMessage::hasChanged(ENERGY_ATTR_U16 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU16.find(attr) != mHasChangedU16.end();
}

}  // namespace aquamqtt::message::next
