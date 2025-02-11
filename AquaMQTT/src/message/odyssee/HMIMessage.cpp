#include "message/odyssee/HMIMessage.h"

#include <cstdio>

namespace aquamqtt::message::odyssee
{

HMIMessage::HMIMessage(uint8_t* data, const uint8_t* previous)
    : mData(data)
    , mHasChangedFloat()
    , mHasChangedBool()
    , mHasChangedU8()
{
    mCreatedWithoutPrevious = previous == nullptr;
    compareWith(previous);
}

void HMIMessage::compareWith(const uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[HMI_MESSAGE_LENGTH_NEXT] = { 0 };
    size_t  numDiffs                             = 0;
    compareBuffers(mData, data, HMI_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        switch (diffIndices[i])
        {
            case 1:
                mHasChangedFloat.insert(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE);
                break;
            default:
                break;
        }
    }
}

uint8_t HMIMessage::getAttr(HMI_ATTR_U8 attr)
{
    return 0;
}

uint16_t HMIMessage::getAttr(HMI_ATTR_U16 attr)
{
    return 0;
}

bool HMIMessage::getAttr(HMI_ATTR_BOOL attr)
{
    return false;
}

float HMIMessage::getAttr(const HMI_ATTR_FLOAT attr)
{
    switch (attr)
    {
        case HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE:
            return mData[1];
    }
    return 0;
}

void HMIMessage::getAttr(HMI_ATTR_STR attr, char* buffer)
{
}

void HMIMessage::setAttr(HMI_ATTR_U8 attr, uint8_t value)
{
}

void HMIMessage::setAttr(HMI_ATTR_BOOL attr, bool value)
{
}

void HMIMessage::setAttr(const HMI_ATTR_FLOAT attr, const float value)
{
    switch (attr)
    {
        case HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE:
            // this heatpump does not support decimals
            mData[1] = static_cast<int>(value);
            break;
    }
}

void HMIMessage::setAttr(HMI_ATTR_U16 attr, uint16_t value)
{
}

bool HMIMessage::hasAttr(HMI_ATTR_U8 attr) const
{
    return false;
}

bool HMIMessage::hasAttr(HMI_ATTR_BOOL attr) const
{
    return false;
}

bool HMIMessage::hasAttr(const HMI_ATTR_FLOAT attr) const
{
    switch (attr)
    {
        case HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE:
            return true;
    }
    return false;
}

bool HMIMessage::hasAttr(HMI_ATTR_U16 attr) const
{
    return false;
}

bool HMIMessage::hasAttr(HMI_ATTR_STR attr) const
{
    return false;
}

bool HMIMessage::hasChanged(const HMI_ATTR_U8 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU8.contains(attr);
}

bool HMIMessage::hasChanged(const HMI_ATTR_BOOL attr) const
{
    return mCreatedWithoutPrevious || mHasChangedBool.contains(attr);
}

bool HMIMessage::hasChanged(const HMI_ATTR_FLOAT attr) const
{
    return mCreatedWithoutPrevious || mHasChangedFloat.contains(attr);
}

bool HMIMessage::hasChanged(const HMI_ATTR_U16 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU16.contains(attr);
}

bool HMIMessage::hasChanged(const HMI_ATTR_STR attr) const
{
    return mCreatedWithoutPrevious || mHasChangedStr.contains(attr);
}

uint8_t HMIMessage::getLength()
{
    return HMI_MESSAGE_LENGTH_ODYSSEE;
}

void HMIMessage::setDateMonthAndYear(uint8_t month, uint16_t year) const
{
}

}  // namespace aquamqtt::message::odyssee
