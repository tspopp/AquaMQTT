#include "message/odyssee/HMIMessage.h"

#include <cstdio>

namespace aquamqtt::message::odyssee
{
HMIMessage::HMIMessage(uint8_t* data, const uint8_t* previous)
    : mHasChangedU8()
    , mHasChangedBool()
    , mHasChangedFloat()
    , mData(data)
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
            case 2:
                mHasChangedU8.insert(HMI_ATTR_U8::OPERATION_TYPE);
                mHasChangedU8.insert(HMI_ATTR_U8::OPERATION_MODE);
                break;
            case 4:
                mHasChangedU8.insert(HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES);
                mHasChangedU8.insert(HMI_ATTR_U8::CONFIG_AIRDUCT);
                break;
            case 5:
                mHasChangedBool.insert(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED);
                break;
            case 20:
                mHasChangedU8.insert(HMI_ATTR_U8::TIME_MINUTES);
                break;
            case 21:
                mHasChangedU8.insert(HMI_ATTR_U8::TIME_HOURS);
                break;
            case 17:
            case 18:
                mHasChangedU8.insert(HMI_ATTR_U8::DATE_DAY);
                mHasChangedU8.insert(HMI_ATTR_U8::DATE_MONTH);
                mHasChangedU16.insert(HMI_ATTR_U16::DATE_YEAR);
                break;
            case 27:
                mHasChangedU8.insert(HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED);
                break;
            case 28:
                mHasChangedU8.insert(HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED);
                break;
            default:
                break;
        }
    }
}

uint8_t HMIMessage::getAttr(const HMI_ATTR_U8 attr)
{
    switch (attr)
    {
        case HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES:
            return mData[4] & 0x0F;
        case HMI_ATTR_U8::TIME_MINUTES:
            return mData[20];
        case HMI_ATTR_U8::TIME_HOURS:
            return mData[21];
        case HMI_ATTR_U8::DATE_DAY:
            return mData[17] & 0x1F;
        case HMI_ATTR_U8::DATE_MONTH:
            return 1 + (mData[17] >> 5) + ((mData[18] % 2) * 8);
        case HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED:
            return mData[27];
        case HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED:
            return mData[28];
        case HMI_ATTR_U8::CONFIG_AIRDUCT:
            switch (mData[4] & 0xF0)
            {
                case 0x00:
                    return AD_INT_INT;
                case 0x10:
                    return AD_INT_EXT;
                case 0x20:
                    return AD_EXT_EXT;
                default:
                    return AD_UNKNOWN;
            }
        case HMI_ATTR_U8::OPERATION_TYPE:
            switch (mData[2] & 0xF0)
            {
                case 0x00:
                    return OT_OFF_PEAK_HOURS;
                case 0x10:
                    return OT_TIMER;
                case 0x40:
                    return OT_ALWAYS_ON;
                default:
                    return OT_UNKNOWN;
            }
        case HMI_ATTR_U8::OPERATION_MODE:
            // unfortunately we cannot parse the lower nibble, since BOOST and ABSENCE have different identifiers
            // influenced by the operation type. For example, in operation type OFF_PEAK_HOURS Boost is 10, while in
            // Timer and Always On it is 8
            switch (mData[2])
            {
                case 10:
                    // off-peak
                case 72:
                    // always on
                case 24:
                    // timer
                    return OM_BOOST;
                case 2:
                case 66:
                case 18:
                    return OM_ECO_INACTIVE;
                case 1:
                case 65:
                case 17:
                    return OM_ECO_ACTIVE;
                case 0:
                case 64:
                case 16:
                    return OM_AUTO;
                case 6:
                case 68:
                case 20:
                    return OM_ABSENCE;
                default:
                    return OM_UNKNOWN;
            }
        default:
            return 0;
    }
}

uint16_t HMIMessage::getAttr(const HMI_ATTR_U16 attr)
{
    switch (attr)
    {
        case HMI_ATTR_U16::DATE_YEAR:
            return 2000 + (mData[18] / 2);
        default:
            return 0;
    }
}

bool HMIMessage::getAttr(const HMI_ATTR_BOOL attr)
{
    switch (attr)
    {
        case HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED:
            return mData[5] & 0x01;
        default:
            return false;
    }
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

bool HMIMessage::hasAttr(const HMI_ATTR_U8 attr) const
{
    switch (attr)
    {
        case HMI_ATTR_U8::TIME_MINUTES:
        case HMI_ATTR_U8::TIME_HOURS:
        case HMI_ATTR_U8::DATE_DAY:
        case HMI_ATTR_U8::DATE_MONTH:
        case HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED:
        case HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED:
        case HMI_ATTR_U8::CONFIG_AIRDUCT:
        case HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES:
        case HMI_ATTR_U8::OPERATION_TYPE:
        case HMI_ATTR_U8::OPERATION_MODE:
            return true;
        default:
            return false;
    }
}

bool HMIMessage::hasAttr(HMI_ATTR_BOOL attr) const
{
    switch (attr)
    {
        case HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED:
            return true;
        default:
            return false;
    }
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

bool HMIMessage::hasAttr(const HMI_ATTR_U16 attr) const
{
    switch (attr)
    {
        case HMI_ATTR_U16::DATE_YEAR:
            return true;
        default:
            return false;
    }
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
