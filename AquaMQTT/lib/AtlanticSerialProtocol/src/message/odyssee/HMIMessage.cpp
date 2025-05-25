#include "message/odyssee/HMIMessage.h"

#include <cstdio>

namespace aquamqtt::message::odyssee
{
HMIMessage::HMIMessage(uint8_t* data, const uint8_t* previous) : mData(data)
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

    uint8_t diffIndices[HMI_MESSAGE_LENGTH_ODYSSEE] = { 0 };
    size_t  numDiffs                                = 0;
    compareBuffers(mData, data, HMI_MESSAGE_LENGTH_ODYSSEE, diffIndices, &numDiffs);

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
            case 19:
                mHasChangedU8.insert(HMI_ATTR_U8::TIME_MINUTES);
                break;
            case 20:
                mHasChangedU8.insert(HMI_ATTR_U8::TIME_HOURS);
                break;
            case 17:
            case 18:
                mHasChangedU8.insert(HMI_ATTR_U8::DATE_DAY);
                mHasChangedU8.insert(HMI_ATTR_U8::DATE_MONTH);
                mHasChangedU16.insert(HMI_ATTR_U16::DATE_YEAR);
                break;
            case 26:
                mHasChangedU8.insert(HMI_ATTR_U8::VERSION_HMI_ASCII);
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

int HMIMessage::getValueByOperationModeAndType(
        const HMIOperationMode operationMode,
        const HMIOperationType operationType)
{
    switch (operationType)
    {
        case OT_ALWAYS_ON:
            switch (operationMode)
            {
                case OM_ABSENCE:
                    return 68;
                case OM_ECO_ACTIVE:
                    return 65;
                case OM_ECO_INACTIVE:
                    return 66;
                case OM_BOOST:
                    return 72;
                case OM_AUTO:
                    return 64;
                default:
                    return 0;
            }
        case OT_TIMER:
            switch (operationMode)
            {
                case OM_ABSENCE:
                    return 20;
                case OM_ECO_ACTIVE:
                    return 17;
                case OM_ECO_INACTIVE:
                    return 18;
                case OM_BOOST:
                    return 24;
                case OM_AUTO:
                    return 16;
                default:
                    return 0;
            }
        case OT_OFF_PEAK_HOURS:
            switch (operationMode)
            {
                case OM_ABSENCE:
                    return 6;
                case OM_ECO_ACTIVE:
                    return 1;
                case OM_ECO_INACTIVE:
                    return 2;
                case OM_BOOST:
                    return 10;
                case OM_AUTO:
                default:
                    return 0;
            }
        default:
            return 0;
    }
}

uint8_t HMIMessage::getAttr(const HMI_ATTR_U8 attr)
{
    switch (attr)
    {
        case HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES:
            return mData[4] & 0x0F;
        case HMI_ATTR_U8::TIME_MINUTES:
            return mData[19];
        case HMI_ATTR_U8::TIME_HOURS:
            return mData[20];
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
                case 72:
                case 24:
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
        case HMI_ATTR_U8::VERSION_HMI_ASCII:
            return mData[26];
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

void HMIMessage::setAttr(const HMI_ATTR_U8 attr, uint8_t value)
{
    switch (attr)
    {
        case HMI_ATTR_U8::TIME_MINUTES:
            mData[19] = value;
            break;
        case HMI_ATTR_U8::TIME_HOURS:
            mData[20] = value;
            break;
        case HMI_ATTR_U8::DATE_DAY:
            mData[17] = (mData[17] & 0xE0) | (value & 0x1F);
            break;
        case HMI_ATTR_U8::CONFIG_AIRDUCT:
        {
            const auto config = static_cast<HMIAirDuctConfig>(value);
            if (config == AD_UNKNOWN)
            {
                return;
            }
            // clear bit 5 and 6 (INT/INT)
            mData[4] &= 0b11001111;
            if (config == AD_INT_EXT)
            {
                mData[4] |= 0b00010000;
            }
            else if (config == AD_EXT_EXT)
            {
                mData[4] |= 0b00100000;
            }
        }
        break;
        case HMI_ATTR_U8::OPERATION_MODE:
        {
            // changing operation mode might have an influence on operation type
            mData[2] = getValueByOperationModeAndType(
                    static_cast<HMIOperationMode>(value),
                    static_cast<HMIOperationType>(getAttr(HMI_ATTR_U8::OPERATION_TYPE)));
        }
        break;
        case HMI_ATTR_U8::OPERATION_TYPE:
        {

            // changing operation type might have an influence on operation type
            mData[2] = getValueByOperationModeAndType(
                    static_cast<HMIOperationMode>(getAttr(HMI_ATTR_U8::OPERATION_MODE)),
                    static_cast<HMIOperationType>(value));
        }
        break;
        case HMI_ATTR_U8::DATE_MONTH:
            // use custom method for setting month and year
        default:
            break;
    }
}

void HMIMessage::setAttr(const HMI_ATTR_BOOL attr, const bool value)
{
    switch (attr)
    {
        case HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED:
            if (value)
            {
                mData[5] |= 0x01;
            }
            else
            {
                mData[5] &= ~0x01;
            }
            break;
        default:
            break;
    }
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

void HMIMessage::setAttr(const HMI_ATTR_U16 attr, uint16_t value)
{
    switch (attr)
    {
        case HMI_ATTR_U16::DATE_YEAR:
            // use custom method for setting month and year
        default:
            break;
    }
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
        case HMI_ATTR_U8::VERSION_HMI_ASCII:
            return true;
        default:
            return false;
    }
}

bool HMIMessage::hasAttr(const HMI_ATTR_BOOL attr) const
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

void HMIMessage::setDateMonthAndYear(const uint8_t month, const uint16_t year) const
{
    const int pastAugust = month > 8 ? 1 : 0;
    mData[18]            = ((year - 2000) * 2) + pastAugust;

    // we always need to reduce by one, since a value of zero is january or september
    const int off_by_one = month - 1;
    // substract -8 in case we are pastAugust
    const int monthValue = (pastAugust ? (off_by_one - 8) : off_by_one) << 5;
    mData[17]            = (mData[17] & 0x1F) | (monthValue & 0xE0);
}
}  // namespace aquamqtt::message::odyssee
