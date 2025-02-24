#include "message/odyssee/MainEnergyMessage.h"

#include "message/MessageConstants.h"

namespace aquamqtt::message::odyssee
{
MainEnergyMessage::MainEnergyMessage(uint8_t* data, const uint8_t* previous)
    : mData(data)
{
    mCreatedWithoutPrevious = (previous == nullptr);
    compareWith(previous);
}

void MainEnergyMessage::compareWith(const uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[ENERGY_MESSAGE_LENGTH_ODYSSEE] = { 0 };
    size_t  numDiffs                                   = 0;
    compareBuffers(mData, data, ENERGY_MESSAGE_LENGTH_ODYSSEE, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        switch (diffIndices[i])
        {
            case 9:
            case 10:
            case 11:
            case 12:
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS);
                break;
            case 13:
            case 14:
            case 15:
            case 16:
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS);
                break;
            default:
                break;
        }
    }
}

uint8_t MainEnergyMessage::getLength()
{
    return ENERGY_MESSAGE_LENGTH_ODYSSEE;
}

uint64_t MainEnergyMessage::getAttr(ENERGY_ATTR_U64 attr)
{
    return 0;
}

uint32_t MainEnergyMessage::getAttr(const ENERGY_ATTR_U32 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS:
            return ((uint32_t) mData[16] << 24) | ((uint32_t) mData[15] << 16) | ((uint32_t) mData[14] << 8)
                   | (uint32_t) mData[13];
        case ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS:
            return ((uint32_t) mData[12] << 24) | ((uint32_t) mData[11] << 16) | ((uint32_t) mData[10] << 8)
                   | (uint32_t) mData[9];
        default:
            return 0;
    }
}

uint16_t MainEnergyMessage::getAttr(ENERGY_ATTR_U16 attr)
{
    return 0;
}

int8_t MainEnergyMessage::getAttr(ENERGY_ATTR_I8 attr)
{
    return 0;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U64 attr) const
{
    return false;
}

bool MainEnergyMessage::hasAttr(const ENERGY_ATTR_U32 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS:
        case ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS:
            return true;
        default:
            return false;
    }
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U16 attr) const
{
    return false;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_I8 attr) const
{
    return false;
}

bool MainEnergyMessage::hasChanged(const ENERGY_ATTR_U64 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU64.contains(attr);
}

bool MainEnergyMessage::hasChanged(const ENERGY_ATTR_U32 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU32.contains(attr);
}

bool MainEnergyMessage::hasChanged(const ENERGY_ATTR_U16 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU16.contains(attr);
}

bool MainEnergyMessage::hasChanged(const ENERGY_ATTR_I8 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedI8.contains(attr);
}
}  // namespace aquamqtt::message::odyssee
