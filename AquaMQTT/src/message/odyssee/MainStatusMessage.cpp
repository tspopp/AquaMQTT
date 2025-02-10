#include "message/odyssee/MainStatusMessage.h"

namespace aquamqtt::message::odyssee
{

MainStatusMessage::MainStatusMessage(uint8_t* data, uint8_t* previous)
    : mData(data)
    , mHasChangedFloat()
    , mHasChangedU8()
    , mHasChangedBool()
{
    mCreatedWithoutPrevious = previous == nullptr;
    compareWith(previous);
}

void MainStatusMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[MAIN_MESSAGE_LENGTH_NEXT] = { 0 };
    size_t  numDiffs                              = 0;
    compareBuffers(mData, data, MAIN_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

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

float MainStatusMessage::getAttr(MAIN_ATTR_FLOAT attr)
{
    return 0;
}

bool MainStatusMessage::getAttr(MAIN_ATTR_BOOL attr)
{
    return false;
}

uint8_t MainStatusMessage::getAttr(MAIN_ATTR_U8 attr)
{
    return 0;
}

uint16_t MainStatusMessage::getAttr(MAIN_ATTR_U16 attr)
{
    return 0;
}

void MainStatusMessage::setAttr(MAIN_ATTR_FLOAT attr, float value)
{
}

void MainStatusMessage::setAttr(MAIN_ATTR_BOOL attr, bool value)
{
}

void MainStatusMessage::setAttr(MAIN_ATTR_U8 attr, uint8_t value)
{
}

void MainStatusMessage::setAttr(MAIN_ATTR_U16 attr, uint16_t value)
{
}

bool MainStatusMessage::hasAttr(MAIN_ATTR_FLOAT attr) const
{
    return false;
}

bool MainStatusMessage::hasAttr(MAIN_ATTR_BOOL attr) const
{
    return false;
}

bool MainStatusMessage::hasAttr(MAIN_ATTR_U8 attr) const
{
    return false;
}

bool MainStatusMessage::hasAttr(MAIN_ATTR_U16 attr) const
{
    return false;
}

bool MainStatusMessage::hasChanged(MAIN_ATTR_FLOAT attr) const
{
    return mCreatedWithoutPrevious || mHasChangedFloat.find(attr) != mHasChangedFloat.end();
}

bool MainStatusMessage::hasChanged(MAIN_ATTR_BOOL attr) const
{
    return mCreatedWithoutPrevious || mHasChangedBool.find(attr) != mHasChangedBool.end();
}

bool MainStatusMessage::hasChanged(MAIN_ATTR_U8 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU8.find(attr) != mHasChangedU8.end();
}

bool MainStatusMessage::hasChanged(MAIN_ATTR_U16 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU16.find(attr) != mHasChangedU16.end();
}

uint8_t MainStatusMessage::getLength()
{
    return MAIN_MESSAGE_LENGTH_ODYSSEE;
}
}  // namespace aquamqtt::message::odyssee
