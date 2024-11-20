#include "message/legacy/HMIMessage.h"

#include <cstdio>

namespace aquamqtt::message::legacy
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

void HMIMessage::setDateMonthAndYear(uint8_t month, uint16_t year) const
{
    int pastJuly   = month > 7 ? 1 : 0;
    mData[19]      = ((year - 2000) * 2) + pastJuly;
    int monthValue = (pastJuly ? month - 8 : month) << 5;
    mData[18]      = (mData[18] & 0x1F) | (monthValue & 0xE0);
}

void HMIMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[HMI_MESSAGE_LENGTH_LEGACY] = { 0 };
    size_t  numDiffs                               = 0;
    compareBuffers(mData, data, HMI_MESSAGE_LENGTH_LEGACY, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 1:
            case 2:
                mHasChangedFloat.insert(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE);
                break;
            case 3:
                mHasChangedU8.insert(HMI_ATTR_U8::OPERATION_MODE);
                mHasChangedU8.insert(HMI_ATTR_U8::OPERATION_TYPE);
                break;
            case 5:
                mHasChangedU8.insert(HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES);
                mHasChangedU8.insert(HMI_ATTR_U8::CONFIG_AIRDUCT);
                break;
            case 6:
                mHasChangedBool.insert(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED);
                break;
            case 8:
                mHasChangedU8.insert(HMI_ATTR_U8::CONFIG_FAN_EXHAUST);
                break;
            case 7:
                mHasChangedU8.insert(HMI_ATTR_U8::STATE_INSTALLATION_MODE);
                break;
            case 9:
                mHasChangedBool.insert(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED);
                mHasChangedU8.insert(HMI_ATTR_U8::STATE_SETUP);
                mHasChangedBool.insert(HMI_ATTR_BOOL::PV_INPUT_ALLOWED);
                break;
            case 10:
                mHasChangedStr.insert(HMI_ATTR_STR::TIMER_WINDOW_A);
                mHasChangedU16.insert(HMI_ATTR_U16::TIMER_A_START);
                break;
            case 11:
                mHasChangedStr.insert(HMI_ATTR_STR::TIMER_WINDOW_A);
                mHasChangedU16.insert(HMI_ATTR_U16::TIMER_A_LENGTH);
                break;
            case 12:
                mHasChangedStr.insert(HMI_ATTR_STR::TIMER_WINDOW_B);
                mHasChangedU16.insert(HMI_ATTR_U16::TIMER_B_START);
                break;
            case 13:
                mHasChangedStr.insert(HMI_ATTR_STR::TIMER_WINDOW_B);
                mHasChangedU16.insert(HMI_ATTR_U16::TIMER_B_LENGTH);
                break;
            case 17:
                mHasChangedU8.insert(HMI_ATTR_U8::TIME_SECONDS);
                break;
            case 20:
                mHasChangedU8.insert(HMI_ATTR_U8::TIME_MINUTES);
                break;
            case 21:
                mHasChangedU8.insert(HMI_ATTR_U8::TIME_HOURS);
                break;
            case 18:
            case 19:
                mHasChangedU8.insert(HMI_ATTR_U8::DATE_DAY);
                mHasChangedU8.insert(HMI_ATTR_U8::DATE_MONTH);
                mHasChangedU16.insert(HMI_ATTR_U16::DATE_YEAR);
                break;
            case 22:
                mHasChangedU8.insert(HMI_ATTR_U8::STATE_TEST);
                break;
            case 28:
                mHasChangedU8.insert(HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED);
                break;
            case 29:
                mHasChangedU8.insert(HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED);
                break;
            default:
                break;
        }
    }
}

uint8_t HMIMessage::getLength()
{
    return HMI_MESSAGE_LENGTH_LEGACY;
}

uint8_t HMIMessage::getAttr(HMI_ATTR_U8 attr)
{
    switch (attr)
    {
        case HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES:
            return (uint8_t) (mData[5] & 0x0F);
        case HMI_ATTR_U8::TIME_SECONDS:
            return mData[17];
        case HMI_ATTR_U8::TIME_MINUTES:
            return mData[20];
        case HMI_ATTR_U8::TIME_HOURS:
            return mData[21];
        case HMI_ATTR_U8::DATE_DAY:
            return mData[18] & 0x1F;
        case HMI_ATTR_U8::DATE_MONTH:
            return (mData[18] >> 5) + ((mData[19] % 2) * 8);

        case HMI_ATTR_U8::STATE_SETUP:
            if (mData[9] & 0x80)
            {
                return HMISetup::RESET;
            }
            if (mData[9] & 0x20)
            {
                return HMISetup::INCOMPLETE;
            }
            return HMISetup::COMPLETED;
        case HMI_ATTR_U8::STATE_TEST:
            switch (mData[22])
            {
                case 0:
                    return TM_OFF;
                case 1:
                    return TM_ENTERED;
                case 2:
                    return TM_HEAT_PUMP;
                case 3:
                    return TM_HEAT_ELEMENT;
                case 4:
                    return TM_FAN_LOW;
                case 5:
                    return TM_FAN_HIGH;
                case 6:
                    return TM_DEFROST;
                case 8:
                    return TM_HEAT_PUMP_AND_EXT;
                default:
                    return TM_UNKNOWN;
            }
        case HMI_ATTR_U8::STATE_INSTALLATION_MODE:
            if (mData[7] & 0x02)
            {
                return INST_HP_AND_SOLAR;
            }
            if (mData[7] & 0x01)
            {
                if (mData[7] & 0x10 && mData[7] & 0x20)
                {
                    return HMIInstallation::INST_HP_AND_EXT_PRIO_EXT;
                }
                if (!(mData[7] & 0x10) && mData[7] & 0x20)
                {
                    return HMIInstallation::INST_HP_AND_EXT_OPT_EXT;
                }
                if (mData[7] & 0x10 && !(mData[7] & 0x20))
                {
                    return HMIInstallation::INST_HP_AND_EXT_OPT_HP;
                }
                return HMIInstallation::INST_HP_AND_EXT_PRIO_HP;
            }
            return HMIInstallation::INST_HP_ONLY;
        case HMI_ATTR_U8::CONFIG_AIRDUCT:
            switch ((uint8_t) (mData[5] & 0xF0))
            {
                case 0:
                    return AD_INT_INT;
                case 16:
                    return AD_INT_EXT;
                case 32:
                    return AD_EXT_EXT;
                default:
                    return AD_UNKNOWN;
            }
        case HMI_ATTR_U8::CONFIG_FAN_EXHAUST:
            switch ((uint8_t) (mData[8] & 0x03))
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
            return mData[29];
        case HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED:
            return mData[28];
        case HMI_ATTR_U8::OPERATION_MODE:
            switch (mData[3] & 0x0F)
            {
                case 0:
                    return OM_ABSENCE;
                case 1:
                    return OM_ECO_ACTIVE;
                case 2:
                    return OM_ECO_INACTIVE;
                case 3:
                    return OM_BOOST;
                case 4:
                    return OM_AUTO;
                default:
                    return OM_UNKNOWN;
            }
        case HMI_ATTR_U8::OPERATION_TYPE:
            if (mData[3] & 0x40)
            {
                return HMIOperationType::TIMER;
            }
            return HMIOperationType::ALWAYS_ON;
    }
    return 0;
}

bool HMIMessage::getAttr(HMI_ATTR_BOOL attr)
{
    switch (attr)
    {
        case HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED:
            return mData[6] & 0x01;
        case HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED:
            return mData[9] & 0x04;
        case HMI_ATTR_BOOL::PV_INPUT_ALLOWED:
            return mData[9] & 0x02;
    }
    return false;
}

float HMIMessage::getAttr(HMI_ATTR_FLOAT attr)
{
    switch (attr)
    {
        case HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE:
            return (float) (((short int) (mData[2] << 8) | mData[1]) / 10.0);
    }
    return 0;
}

uint16_t HMIMessage::getAttr(HMI_ATTR_U16 attr)
{
    switch (attr)
    {
        case HMI_ATTR_U16::TIMER_A_START:
            return mData[10];
        case HMI_ATTR_U16::TIMER_A_LENGTH:
            return mData[11];
        case HMI_ATTR_U16::TIMER_B_START:
            return mData[12];
        case HMI_ATTR_U16::TIMER_B_LENGTH:
            return mData[13];
        case HMI_ATTR_U16::DATE_YEAR:
            return 2000 + (mData[19] / 2);
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
            const uint8_t beginHours    = (start * 15) / 60;
            const uint8_t beginMinutes  = (start * 15) % 60;
            const uint8_t lengthHours   = (duration * 15) / 60;
            const uint8_t lengthMinutes = (duration * 15) % 60;
            const uint8_t endMinutes    = (beginMinutes + lengthMinutes) % 60;
            const uint8_t hourOverlap   = (beginMinutes + lengthMinutes) / 60;
            const uint8_t endHours      = (beginHours + lengthHours) % 24 + hourOverlap;
            sprintf(buffer, "%02d:%02d-%02d:%02d", beginHours, beginMinutes, endHours, endMinutes);
            break;
    }
}

void HMIMessage::setAttr(HMI_ATTR_U8 attr, uint8_t value)
{
    switch (attr)
    {
        case HMI_ATTR_U8::TIME_SECONDS:
            mData[17] = value;
            break;
        case HMI_ATTR_U8::TIME_MINUTES:
            mData[20] = value;
            break;
        case HMI_ATTR_U8::TIME_HOURS:
            mData[21] = value;
            break;
        case HMI_ATTR_U8::DATE_DAY:
            mData[18] = (mData[18] & 0xE0) | (value & 0x1F);
            break;
        case HMI_ATTR_U8::STATE_INSTALLATION_MODE:
        {
            auto mode = static_cast<HMIInstallation>(value);
            switch (mode)
            {
                case INST_HP_ONLY:
                    mData[7] &= ~0x01;
                    mData[7] &= ~0x02;
                    mData[7] &= ~0x10;
                    mData[7] &= ~0x20;
                    break;
                case INST_HP_AND_EXT_PRIO_HP:
                    mData[7] |= 0x01;
                    mData[7] &= ~0x02;
                    mData[7] &= ~0x10;
                    mData[7] &= ~0x20;
                    break;
                case INST_HP_AND_EXT_OPT_HP:
                    mData[7] |= 0x01;
                    mData[7] &= ~0x02;
                    mData[7] |= 0x10;
                    mData[7] &= ~0x20;
                    break;
                case INST_HP_AND_EXT_OPT_EXT:
                    mData[7] |= 0x01;
                    mData[7] &= ~0x02;
                    mData[7] &= ~0x10;
                    mData[7] |= 0x20;
                    break;
                case INST_HP_AND_EXT_PRIO_EXT:
                    mData[7] |= 0x01;
                    mData[7] &= ~0x02;
                    mData[7] |= 0x10;
                    mData[7] |= 0x20;
                    break;
                case INST_HP_AND_SOLAR:
                    mData[7] |= 0x02;
                    mData[7] &= ~0x01;
                    mData[7] &= ~0x10;
                    mData[7] &= ~0x20;
                    break;
                default:
                    break;
            }
        }
        break;
        case HMI_ATTR_U8::CONFIG_AIRDUCT:
        {
            auto config = static_cast<HMIAirDuctConfig>(value);
            if (config == AD_UNKNOWN)
            {
                return;
            }

            // clear bit 5 and 6 (INT/INT)
            mData[5] &= 0b11001111;

            if (config == AD_INT_EXT)
            {
                mData[5] |= 0b00010000;
            }
            else if (config == AD_EXT_EXT)
            {
                mData[5] |= 0b00100000;
            }
        }
        break;
        case HMI_ATTR_U8::CONFIG_FAN_EXHAUST:
        {
            auto mode = static_cast<HMIFanExhaust>(value);
            switch (mode)
            {
                case EXHAUST_STOP:
                    mData[8] &= 0b11111100;
                    mData[8] |= 0;
                    break;
                case EXHAUST_LOW_SPEED:
                    mData[8] &= 0b11111100;
                    mData[8] |= 1;
                    break;
                case EXHAUST_HIGH_SPEED:
                    mData[8] &= 0b11111100;
                    mData[8] |= 2;
                    break;
                case EXHAUST_UNKNOWN:
                    break;
            }
        }
        break;
        case HMI_ATTR_U8::OPERATION_MODE:
        {
            auto operationMode = static_cast<HMIOperationMode>(value);
            if (operationMode != HMIOperationMode::OM_UNKNOWN)
            {
                mData[3] = (mData[3] & 0xF0) | (operationMode & 0x0F);
            }
        }
        break;
        case HMI_ATTR_U8::OPERATION_TYPE:
        {
            auto operationType = static_cast<HMIOperationType>(value);
            if (operationType == HMIOperationType::TIMER)
            {
                mData[3] = (mData[3] & ~(1 << 6)) | (true << 6);
            }
            else
            {
                mData[3] = (mData[3] & ~(1 << 6)) | (false << 6);
            }
        }
        break;
        case HMI_ATTR_U8::DATE_MONTH:
            // use custom method for setting month and year
        case HMI_ATTR_U8::STATE_SETUP:
        case HMI_ATTR_U8::STATE_TEST:
        case HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED:
        case HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED:
        case HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES:
            break;
    }
}

void HMIMessage::setAttr(HMI_ATTR_BOOL attr, bool value)
{
    switch (attr)
    {
        case HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED:
            // Sanity: You cannot activate emergency mode if heating element is disabled
            if (value && getAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED))
            {
                mData[6] |= 0x01;
            }
            else if (!value)
            {
                mData[6] &= ~0x01;
            }
            break;
        case HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED:
            if (value)
            {
                mData[9] |= 0x04;
            }
            // Sanity: You cannot disable heating element if emergency mode is activated
            else if (!getAttr(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED))
            {
                mData[9] &= ~0x04;
            }

            break;
        case HMI_ATTR_BOOL::PV_INPUT_ALLOWED:
            break;
    }
}

void HMIMessage::setAttr(HMI_ATTR_FLOAT attr, float value)
{
    switch (attr)
    {
        case HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE:
        {
            short int rawValue = value * 100 / 10;
            mData[1]           = rawValue & 0xFF;
            mData[2]           = (rawValue >> 8) & 0xFF;
        }
        break;
    }
}

void HMIMessage::setAttr(HMI_ATTR_U16 attr, uint16_t value)
{
    switch (attr)
    {
        case HMI_ATTR_U16::TIMER_A_START:
            break;
        case HMI_ATTR_U16::TIMER_A_LENGTH:
            break;
        case HMI_ATTR_U16::TIMER_B_START:
            break;
        case HMI_ATTR_U16::TIMER_B_LENGTH:
            break;
    }
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
        case HMI_ATTR_U8::STATE_SETUP:
        case HMI_ATTR_U8::STATE_TEST:
        case HMI_ATTR_U8::STATE_INSTALLATION_MODE:
        case HMI_ATTR_U8::CONFIG_AIRDUCT:
        case HMI_ATTR_U8::CONFIG_FAN_EXHAUST:
        case HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED:
        case HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED:
        case HMI_ATTR_U8::OPERATION_MODE:
        case HMI_ATTR_U8::OPERATION_TYPE:
            return true;
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

}  // namespace aquamqtt::message::legacy
