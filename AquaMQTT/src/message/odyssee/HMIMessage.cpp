#include "message/odyssee/HMIMessage.h"

#include <cstdio>

namespace aquamqtt::message::odyssee
{

HMIMessage::HMIMessage(uint8_t* data, uint8_t* previous)
    : mData(data)
    , mHasChangedFloat()
    , mHasChangedBool()
    , mHasChangedU8()
{
    mCreatedWithoutPrevious = previous == nullptr;
    compareWith(previous);
}

void HMIMessage::compareWith(uint8_t* data)
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
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
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

float HMIMessage::getAttr(HMI_ATTR_FLOAT attr)
{
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

void HMIMessage::setAttr(HMI_ATTR_FLOAT attr, float value)
{

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

bool HMIMessage::hasAttr(HMI_ATTR_FLOAT attr) const
{
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

bool HMIMessage::hasChanged(HMI_ATTR_U8 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU8.find(attr) != mHasChangedU8.end();
}

bool HMIMessage::hasChanged(HMI_ATTR_BOOL attr) const
{
    return mCreatedWithoutPrevious || mHasChangedBool.find(attr) != mHasChangedBool.end();
}

bool HMIMessage::hasChanged(HMI_ATTR_FLOAT attr) const
{
    return mCreatedWithoutPrevious || mHasChangedFloat.find(attr) != mHasChangedFloat.end();
}

bool HMIMessage::hasChanged(HMI_ATTR_U16 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU16.find(attr) != mHasChangedU16.end();
}

bool HMIMessage::hasChanged(HMI_ATTR_STR attr) const
{
    return mCreatedWithoutPrevious || mHasChangedStr.find(attr) != mHasChangedStr.end();
}

uint8_t HMIMessage::getLength()
{
    return HMI_MESSAGE_LENGTH_ODYSSEE;
}

void HMIMessage::setDateMonthAndYear(uint8_t month, uint16_t year) const
{
}

}  // namespace aquamqtt::message::odyssee
