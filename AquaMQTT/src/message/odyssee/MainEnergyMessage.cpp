#include "message/odyssee/MainEnergyMessage.h"

#include "message/MessageConstants.h"

namespace aquamqtt::message::odyssee
{

MainEnergyMessage::MainEnergyMessage(uint8_t* data, uint8_t* previous)
    : mData(data)
    , mHasChangedU16()
    , mHasChangedU32()
    , mHasChangedU64()
    , mHasChangedI8()
{
    mCreatedWithoutPrevious = (previous == nullptr);
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

uint32_t MainEnergyMessage::getAttr(ENERGY_ATTR_U32 attr)
{
    return 0;
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

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U32 attr) const
{
    return false;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U16 attr) const
{
    return false;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_I8 attr) const
{
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

bool MainEnergyMessage::hasChanged(ENERGY_ATTR_I8 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedI8.find(attr) != mHasChangedI8.end();
}

}  // namespace aquamqtt::message::odyssee
