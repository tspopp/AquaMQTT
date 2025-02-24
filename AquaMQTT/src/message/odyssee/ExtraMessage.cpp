#include "message/odyssee/ExtraMessage.h"

namespace aquamqtt::message::odyssee
{

ExtraMessage::ExtraMessage(uint8_t* data, const uint8_t* previous) : mData(data)
{
    mCreatedWithoutPrevious = (previous == nullptr);
    compareWith(previous);
}

void ExtraMessage::compareWith(const uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[EXTRA_MESSAGE_LENGTH_ODYSSEE] = { 0 };
    size_t  numDiffs                                  = 0;
    compareBuffers(mData, data, EXTRA_MESSAGE_LENGTH_ODYSSEE, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        switch (diffIndices[i])
        {
            case 1:
            case 2:
                mHasChangedFloat.insert(EXTRA_ATTR_FLOAT::EXTRA_AMPERAGE);
                break;
            case 3:
            case 4:
                mHasChangedU16.insert(EXTRA_ATTR_U16::EXTRA_VOLTAGE_GRID);
                break;
            case 5:
            case 6:
                mHasChangedU16.insert(EXTRA_ATTR_U16::EXTRA_POWER_TOTAL);
                break;
            case 9:
            case 10:
            case 11:
            case 12:
                mHasChangedU32.insert(EXTRA_ATTR_U32::EXTRA_TOTAL_ENERGY);
                break;
            default:
                break;
        }
    }
}

uint8_t ExtraMessage::getLength()
{
    return EXTRA_MESSAGE_LENGTH_ODYSSEE;
}

uint16_t ExtraMessage::getAttr(const EXTRA_ATTR_U16 attr)
{
    switch (attr)
    {
        case EXTRA_ATTR_U16::EXTRA_VOLTAGE_GRID:
            return ((uint16_t) mData[4] << 8) | (uint16_t) mData[3];
        case EXTRA_ATTR_U16::EXTRA_POWER_TOTAL:
            return ((uint16_t) mData[6] << 8) | (uint16_t) mData[5];
        default:
            return 0;
    }
}

uint32_t ExtraMessage::getAttr(const EXTRA_ATTR_U32 attr)
{
    switch (attr)
    {
        case EXTRA_ATTR_U32::EXTRA_TOTAL_ENERGY:
            return ((uint32_t) mData[12] << 24) | ((uint32_t) mData[11] << 16) | ((uint32_t) mData[10] << 8)
                   | (uint32_t) mData[9];
        default:
            return 0;
    }
}

float ExtraMessage::getAttr(const EXTRA_ATTR_FLOAT attr)
{
    switch (attr)
    {
        case EXTRA_ATTR_FLOAT::EXTRA_AMPERAGE:
            return (float) ((mData[2] << 8 | mData[1]) / 100.0);
        default:
            return 0;
    }
}

bool ExtraMessage::hasAttr(const EXTRA_ATTR_U16 attr) const
{
    switch (attr)
    {
        case EXTRA_ATTR_U16::EXTRA_POWER_TOTAL:
        case EXTRA_ATTR_U16::EXTRA_VOLTAGE_GRID:
            return true;
        default:
            return false;
    }
}

bool ExtraMessage::hasAttr(const EXTRA_ATTR_U32 attr) const
{
    switch (attr)
    {
        case EXTRA_ATTR_U32::EXTRA_TOTAL_ENERGY:
            return true;
        default:
            return false;
    }
}

bool ExtraMessage::hasAttr(const EXTRA_ATTR_FLOAT attr) const
{
    switch (attr)
    {
        case EXTRA_ATTR_FLOAT::EXTRA_AMPERAGE:
            return true;
        default:
            return false;
    }
}

bool ExtraMessage::hasChanged(const EXTRA_ATTR_U16 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU16.contains(attr);
}

bool ExtraMessage::hasChanged(const EXTRA_ATTR_U32 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU32.contains(attr);
}

bool ExtraMessage::hasChanged(const EXTRA_ATTR_FLOAT attr) const
{
    return mCreatedWithoutPrevious || mHasChangedFloat.contains(attr);
}

}  // namespace aquamqtt::message::odyssee