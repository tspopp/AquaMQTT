#include "message/next/HMIMessage.h"

#include <cstdio>

namespace aquamqtt::message::next
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
            case 1:
                mHasChangedFloat.insert(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE);
                break;
            case 2:
                mHasChangedU8.insert(HMI_ATTR_U8::OPERATION_MODE);
                break;
            case 3:
                mHasChangedU8.insert(HMI_ATTR_U8::OPERATION_TYPE);
                break;
            case 4:
                mHasChangedU8.insert(HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES);
                break;
            case 5:
                mHasChangedBool.insert(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED);
                break;
            case 6:
                mHasChangedU8.insert(HMI_ATTR_U8::STATE_INSTALLATION_MODE);
                break;
            case 7:
                mHasChangedU8.insert(HMI_ATTR_U8::CONFIG_FAN_EXHAUST);
                break;
            case 8:
                mHasChangedBool.insert(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED);
                mHasChangedBool.insert(HMI_ATTR_BOOL::PV_INPUT_ALLOWED);
                break;
            case 9:
            case 10:
                mHasChangedU16.insert(HMI_ATTR_U16::TIMER_A_START);
                mHasChangedStr.insert(HMI_ATTR_STR::TIMER_WINDOW_A);
                break;
            case 11:
            case 12:
                mHasChangedU16.insert(HMI_ATTR_U16::TIMER_A_LENGTH);
                mHasChangedStr.insert(HMI_ATTR_STR::TIMER_WINDOW_A);
                break;
            case 29:
            case 30:
                mHasChangedU16.insert(HMI_ATTR_U16::TIMER_B_START);
                mHasChangedStr.insert(HMI_ATTR_STR::TIMER_WINDOW_B);
                break;
            case 31:
            case 32:
                mHasChangedU16.insert(HMI_ATTR_U16::TIMER_B_LENGTH);
                mHasChangedStr.insert(HMI_ATTR_STR::TIMER_WINDOW_B);
                break;
            case 16:
                mHasChangedU8.insert(HMI_ATTR_U8::TIME_SECONDS);
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
            case 27:
                mHasChangedU8.insert(HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED);
                break;
            case 28:
                mHasChangedU8.insert(HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED);
            default:
                break;
        }
    }
}

uint8_t HMIMessage::getAttr(HMI_ATTR_U8 attr)
{
    switch (attr)
    {
        case HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES:
            return mData[4] & 0x0F;
        case HMI_ATTR_U8::TIME_SECONDS:
            return mData[16];
        case HMI_ATTR_U8::TIME_MINUTES:
            return mData[19];
        case HMI_ATTR_U8::TIME_HOURS:
            return mData[20];
        case HMI_ATTR_U8::DATE_DAY:
            return mData[17] & 0x1F;
        case HMI_ATTR_U8::DATE_MONTH:
            return 1 + (mData[17] >> 5) + ((mData[18] % 2) * 8);
        case HMI_ATTR_U8::STATE_INSTALLATION_MODE:
            if (mData[6] & 0x02)
            {
                return INST_HP_AND_SOLAR;
            }
            if (mData[6] & 0x01)
            {
                if (mData[6] & 0x10 && mData[6] & 0x20)
                {
                    return HMIInstallation::INST_HP_AND_EXT_PRIO_EXT;
                }
                if (!(mData[6] & 0x10) && mData[6] & 0x20)
                {
                    return HMIInstallation::INST_HP_AND_EXT_OPT_EXT;
                }
                if (mData[6] & 0x10 && !(mData[6] & 0x20))
                {
                    return HMIInstallation::INST_HP_AND_EXT_OPT_HP;
                }
                return HMIInstallation::INST_HP_AND_EXT_PRIO_HP;
            }
            return HMIInstallation::INST_HP_ONLY;
        case HMI_ATTR_U8::CONFIG_AIRDUCT:
            switch (mData[4] & 0xF0)
            {
                case 0:
                    return AD_INT_INT;
                case 1:
                    return AD_INT_EXT;
                case 2:
                    return AD_EXT_EXT;
                default:
                    return AD_UNKNOWN;
            }
        case HMI_ATTR_U8::CONFIG_FAN_EXHAUST:
            switch ((uint8_t) (mData[7] & 0x03))
            {
                case 0:
                    return EXHAUST_STOP;
                case 1:
                    return EXHAUST_LOW_SPEED;
                case 2:
                    return EXHAUST_HIGH_SPEED;
                default:
                    return EXHAUST_UNKNOWN;
            }
        case HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED:
            return mData[28];
        case HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED:
            return mData[27];
        case HMI_ATTR_U8::OPERATION_MODE:
            switch (mData[2] & 0x0F)
            {
                case 0:
                    return OM_AUTO;
                case 1:
                    return OM_ECO_ACTIVE;
                case 2:
                    return OM_ECO_INACTIVE;
                case 9:
                    return OM_BOOST;
                case 5:
                    return OM_ABSENCE;
                default:
                    return OM_UNKNOWN;
            }
        case HMI_ATTR_U8::OPERATION_TYPE:
            if (mData[3] & 0x40)
            {
                return HMIOperationType::ALWAYS_ON;
            }
            return HMIOperationType::TIMER;
        case HMI_ATTR_U8::STATE_SETUP:
        case HMI_ATTR_U8::STATE_TEST:
            break;
    }
    return 0;
}

uint16_t HMIMessage::getAttr(HMI_ATTR_U16 attr)
{
    switch (attr)
    {
        case HMI_ATTR_U16::TIMER_A_START:
            return ((uint16_t) mData[10] << 8) | (uint16_t) mData[9];
        case HMI_ATTR_U16::TIMER_A_LENGTH:
            return ((uint16_t) mData[12] << 8) | (uint16_t) mData[11];
        case HMI_ATTR_U16::TIMER_B_START:
            return ((uint16_t) mData[30] << 8) | (uint16_t) mData[29];
        case HMI_ATTR_U16::TIMER_B_LENGTH:
            return ((uint16_t) mData[32] << 8) | (uint16_t) mData[31];
        case HMI_ATTR_U16::DATE_YEAR:
            return 2000 + (mData[18] / 2);
    }
    return 0;
}

bool HMIMessage::getAttr(HMI_ATTR_BOOL attr)
{
    switch (attr)
    {
        case HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED:
            return mData[5] & 0x01;
        case HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED:
            return mData[8] & 0x04;
        case HMI_ATTR_BOOL::PV_INPUT_ALLOWED:
            return mData[8] & 0x02;
    }
    return false;
}

float HMIMessage::getAttr(HMI_ATTR_FLOAT attr)
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
    switch (attr)
    {
        case HMI_ATTR_STR::TIMER_WINDOW_A:
        case HMI_ATTR_STR::TIMER_WINDOW_B:
            uint16_t      start         = attr == HMI_ATTR_STR::TIMER_WINDOW_A ? getAttr(HMI_ATTR_U16::TIMER_A_START)
                                                                               : getAttr(HMI_ATTR_U16::TIMER_B_START);
            uint16_t      duration      = attr == HMI_ATTR_STR::TIMER_WINDOW_A ? getAttr(HMI_ATTR_U16::TIMER_A_LENGTH)
                                                                               : getAttr(HMI_ATTR_U16::TIMER_B_LENGTH);
            const uint8_t beginHours    = start / 60;
            const uint8_t beginMinutes  = start % 60;
            const uint8_t lengthHours   = duration / 60;
            const uint8_t lengthMinutes = duration % 60;
            const uint8_t endMinutes    = (beginMinutes + lengthMinutes) % 60;
            const uint8_t hourOverlap   = (beginMinutes + lengthMinutes) / 60;
            const uint8_t endHours      = (beginHours + lengthHours) % 24 + hourOverlap;
            sprintf(buffer, "%02d:%02d-%02d:%02d", beginHours, beginMinutes, endHours, endMinutes);
            break;
    }
}

void HMIMessage::setAttr(HMI_ATTR_U8 attr, uint8_t value)
{
    // TODO: not yet implemented
    // void HMIMessage::setOperationMode(HMIOperationMode operationMode) const
    //{
    //     // TODO: Sanity: If Mode OM_AUTO is set, water target temperature is set to 0x00
    // }
}

void HMIMessage::setAttr(HMI_ATTR_BOOL attr, bool value)
{
    // TODO: not yet implemented
}

void HMIMessage::setAttr(HMI_ATTR_FLOAT attr, float value)
{
    // TODO: not yet implemented
}

void HMIMessage::setAttr(HMI_ATTR_U16 attr, uint16_t value)
{
    // TODO: not yet implemented
}

bool HMIMessage::hasAttr(HMI_ATTR_U8 attr) const
{
    switch (attr)
    {
        case HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES:
        case HMI_ATTR_U8::TIME_SECONDS:
        case HMI_ATTR_U8::TIME_MINUTES:
        case HMI_ATTR_U8::TIME_HOURS:
        case HMI_ATTR_U8::DATE_DAY:
        case HMI_ATTR_U8::DATE_MONTH:
        case HMI_ATTR_U8::STATE_INSTALLATION_MODE:
        case HMI_ATTR_U8::CONFIG_AIRDUCT:
        case HMI_ATTR_U8::CONFIG_FAN_EXHAUST:
        case HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED:
        case HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED:
        case HMI_ATTR_U8::OPERATION_MODE:
        case HMI_ATTR_U8::OPERATION_TYPE:
            return true;
        case HMI_ATTR_U8::STATE_SETUP:
        case HMI_ATTR_U8::STATE_TEST:
            return false;
    }
    return false;
}

bool HMIMessage::hasAttr(HMI_ATTR_BOOL attr) const
{
    switch (attr)
    {
        case HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED:
        case HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED:
        case HMI_ATTR_BOOL::PV_INPUT_ALLOWED:
            return true;
    }
    return false;
}

bool HMIMessage::hasAttr(HMI_ATTR_FLOAT attr) const
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
    switch (attr)
    {
        case HMI_ATTR_U16::TIMER_A_START:
        case HMI_ATTR_U16::TIMER_A_LENGTH:
        case HMI_ATTR_U16::TIMER_B_START:
        case HMI_ATTR_U16::TIMER_B_LENGTH:
        case HMI_ATTR_U16::DATE_YEAR:
            return true;
    }
    return false;
}

bool HMIMessage::hasAttr(HMI_ATTR_STR attr) const
{
    switch (attr)
    {
        case HMI_ATTR_STR::TIMER_WINDOW_A:
        case HMI_ATTR_STR::TIMER_WINDOW_B:
            return true;
    }
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
    return HMI_MESSAGE_LENGTH_NEXT;
}

void HMIMessage::setDateMonthAndYear(uint8_t month, uint16_t year) const
{
    // TODO: implement me
}

}  // namespace aquamqtt::message::next
