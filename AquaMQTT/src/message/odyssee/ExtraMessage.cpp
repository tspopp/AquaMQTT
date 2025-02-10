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

    uint8_t diffIndices[ENERGY_MESSAGE_LENGTH_NEXT] = { 0 };
    size_t  numDiffs                                = 0;
    compareBuffers(mData, data, ENERGY_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        switch (diffIndices[i])
        {
            case 3:
            case 4:
                mHasChangedU16.insert(EXTRA_ATTR_U16::EXTRA_VOLTAGE_GRID);
                break;
            case 5:
            case 6:
                mHasChangedU16.insert(EXTRA_ATTR_U16::EXTRA_POWER_TOTAL);
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

bool ExtraMessage::hasChanged(const EXTRA_ATTR_U16 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU16.contains(attr);
}

}  // namespace aquamqtt::message::odyssee