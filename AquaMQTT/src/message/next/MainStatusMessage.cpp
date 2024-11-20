#include "message/next/MainStatusMessage.h"

namespace aquamqtt::message::next
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
            case 1:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::WATER_TEMPERATURE);
                break;
            case 3:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE);
                break;
            case 5:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE);
                break;
            case 7:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE);
                break;
            case 9:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::AIR_TEMPERATURE);
                break;
            default:
                break;
        }
    }
}

float MainStatusMessage::getAttr(MAIN_ATTR_FLOAT attr)
{
    switch (attr)
    {
        case MAIN_ATTR_FLOAT::WATER_TEMPERATURE:
            return mData[1];
        case MAIN_ATTR_FLOAT::AIR_TEMPERATURE:
            return mData[9];
        case MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE:
            return mData[5];
        case MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE:
            return mData[7];
        case MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE:
            return mData[3];
        case MAIN_ATTR_FLOAT::FAN_SPEED_PWM:
            break;
    }
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
    // TODO: reimplement filter for temp values, kalman filter tries to store updated values here
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
    switch (attr)
    {
        case MAIN_ATTR_FLOAT::WATER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::AIR_TEMPERATURE:
        case MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE:
            return true;
        case MAIN_ATTR_FLOAT::FAN_SPEED_PWM:
            return false;
    }
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
    return MAIN_MESSAGE_LENGTH_NEXT;
}
}  // namespace aquamqtt::message::next
