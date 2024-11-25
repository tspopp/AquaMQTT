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
            case 18:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::FAN_SPEED_PWM);
                break;
            case 17:
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_HEATPUMP);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_BOILER_BACKUP);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_FAN);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_DEFROST);
                break;
            case 22:
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_PV);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_SOLAR);
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
            return mData[18];
    }
    return 0;
}

bool MainStatusMessage::getAttr(MAIN_ATTR_BOOL attr)
{
    switch (attr)
    {
        case MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT:
            return mData[17] & 0x01;
        case MAIN_ATTR_BOOL::STATE_HEATPUMP:
            return mData[17] & 0x02;
        case MAIN_ATTR_BOOL::STATE_BOILER_BACKUP:
            return mData[17] & 0x04;
        case MAIN_ATTR_BOOL::STATE_FAN:
            return mData[17] & 0x08;
        case MAIN_ATTR_BOOL::STATE_DEFROST:
            return mData[17] & 0x20;
        case MAIN_ATTR_BOOL::STATE_PV:
            return mData[22] & 0x10;
        case MAIN_ATTR_BOOL::STATE_SOLAR:
            return mData[22] & 0x20;
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_HEAT_EXCHANGER:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING:
            break;
    }
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
    switch (attr)
    {
        case MAIN_ATTR_FLOAT::WATER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::AIR_TEMPERATURE:
        case MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE:
        case MAIN_ATTR_FLOAT::FAN_SPEED_PWM:
            return true;
    }
    return false;
}

bool MainStatusMessage::hasAttr(MAIN_ATTR_BOOL attr) const
{
    switch (attr)
    {
        case MAIN_ATTR_BOOL::STATE_PV:
        case MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT:
        case MAIN_ATTR_BOOL::STATE_HEATPUMP:
        case MAIN_ATTR_BOOL::STATE_BOILER_BACKUP:
        case MAIN_ATTR_BOOL::STATE_FAN:
        case MAIN_ATTR_BOOL::STATE_DEFROST:
        case MAIN_ATTR_BOOL::STATE_SOLAR:
            return true;
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_HEAT_EXCHANGER:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING:
            break;
    }
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
