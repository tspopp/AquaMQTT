#include "message/legacy/MainStatusMessage.h"

namespace aquamqtt::message::legacy
{

void MainStatusMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[MAIN_MESSAGE_LENGTH_LEGACY] = { 0 };
    size_t  numDiffs                                = 0;
    compareBuffers(mData, data, MAIN_MESSAGE_LENGTH_LEGACY, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 1:
            case 2:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::WATER_TEMPERATURE);
                break;
            case 3:
            case 4:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::AIR_TEMPERATURE);
                break;
            case 5:
            case 6:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE);
                break;
            case 7:
            case 8:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE);
                break;
            case 14:
                mHasChangedU8.insert(MAIN_ATTR_U8::SETTING_FAN_PWM_THIRD);
                break;
            case 15:
                mHasChangedU8.insert(MAIN_ATTR_U8::SETTING_FAN_PWM_FIRST);
                break;
            case 16:
                mHasChangedU8.insert(MAIN_ATTR_U8::SETTING_FAN_PWM_SECOND);
                break;
            case 17:
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_HEATPUMP);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_BOILER_BACKUP);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_FAN);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_DEFROST);
                break;
            case 18:
            case 19:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::FAN_SPEED_PWM);
                break;
            case 20:
                mHasChangedU8.insert(MAIN_ATTR_U8::SETTING_MIN_TARGET_WATER_TEMPERATURE);
                break;
            case 21:
                mHasChangedU8.insert(MAIN_ATTR_U8::SETTING_LEGIONELLA_TARGET_WATER_TEMPERATURE);
                break;
            case 22:
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_PV);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::STATE_SOLAR);
                break;
            case 23:
                mHasChangedU8.insert(MAIN_ATTR_U8::ERROR_CODE);
                break;
            case 32:
                mHasChangedU16.insert(MAIN_ATTR_U16::SETTING_HEAT_ELEMENT_WATTAGE);
                break;
            case 33:
            case 34:
                mHasChangedU16.insert(MAIN_ATTR_U16::SETTING_BOILER_CAPACITY);
                break;
            case 35:
                mHasChangedU8.insert(MAIN_ATTR_U8::SETTING_BRAND);
                break;
            case 36:
                mHasChangedBool.insert(MAIN_ATTR_BOOL::CAPABILITY_HAS_HEAT_EXCHANGER);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION);
                mHasChangedBool.insert(MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING);
                break;
            default:
                break;
        }
    }
}

MainStatusMessage::MainStatusMessage(uint8_t* data, uint8_t* previous)
    : mData(data)
    , mHasChangedU16()
    , mHasChangedBool()
    , mHasChangedFloat()
    , mHasChangedU8()
{
    mCreatedWithoutPrevious = previous == nullptr;
    compareWith(previous);
}

uint8_t MainStatusMessage::getLength()
{
    return MAIN_MESSAGE_LENGTH_LEGACY;
}

float MainStatusMessage::getAttr(MAIN_ATTR_FLOAT attr)
{
    switch (attr)
    {
        case MAIN_ATTR_FLOAT::WATER_TEMPERATURE:
            return (float) (((short int) (mData[2] << 8) | mData[1]) / 10.0);
        case MAIN_ATTR_FLOAT::AIR_TEMPERATURE:
            return (float) (((short int) (mData[4] << 8) | mData[3]) / 10.0);
        case MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE:
            return (float) (((short int) (mData[8] << 8) | mData[7]) / 10.0);
        case MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE:
            return (float) (((short int) (mData[6] << 8) | mData[5]) / 10.0);
        case MAIN_ATTR_FLOAT::FAN_SPEED_PWM:
            return (float) (((short int) (mData[19] << 8) | mData[18]) / 10.0);
        case MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE:
            break;
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
            return mData[36] & 0x01;
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION:
            return mData[36] & 0x02;
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT:
            return mData[36] & 0x04;
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION:
            return !(mData[36] & 0x08);
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING:
            return (mData[36] & 0x20);
    }
    return false;
}

uint8_t MainStatusMessage::getAttr(MAIN_ATTR_U8 attr)
{
    switch (attr)
    {
        case MAIN_ATTR_U8::ERROR_CODE:
            if (mData[23] == UINT8_MAX)
            {
                return 0;
            }
            return mData[23];
        case MAIN_ATTR_U8::SETTING_FAN_PWM_FIRST:
            return mData[15];
        case MAIN_ATTR_U8::SETTING_FAN_PWM_SECOND:
            return mData[16];
        case MAIN_ATTR_U8::SETTING_FAN_PWM_THIRD:
            return mData[14];
        case MAIN_ATTR_U8::SETTING_MIN_TARGET_WATER_TEMPERATURE:
            return mData[20];
        case MAIN_ATTR_U8::SETTING_LEGIONELLA_TARGET_WATER_TEMPERATURE:
            return mData[21];
        case MAIN_ATTR_U8::SETTING_BRAND:
            switch (mData[35])
            {
                case 65:
                    return MAINBrands::BR_ATLANTIC;
                case 78:
                    return MAINBrands::BR_NONAME;
                case 83:
                    return MAINBrands::BR_SAUTER;
                case 84:
                    return MAINBrands::BR_THERMOR;
                default:
                    return MAINBrands::BR_UNKNOWN;
            }
    }
    return 0;
}

uint16_t MainStatusMessage::getAttr(MAIN_ATTR_U16 attr)
{
    switch (attr)
    {
        case MAIN_ATTR_U16::SETTING_HEAT_ELEMENT_WATTAGE:
            return mData[32] * 100;
        case MAIN_ATTR_U16::SETTING_BOILER_CAPACITY:
            return ((uint16_t) mData[34] << 8) | (uint16_t) mData[33];
    }
    return 0;
}

void MainStatusMessage::setAttr(MAIN_ATTR_FLOAT attr, float value)
{
    switch (attr)
    {
        case MAIN_ATTR_FLOAT::WATER_TEMPERATURE:
        {
            short int rawValue = value * 100 / 10;
            mData[1]           = rawValue & 0xFF;
            mData[2]           = (rawValue >> 8) & 0xFF;
        }
        break;
        case MAIN_ATTR_FLOAT::AIR_TEMPERATURE:
        {
            short int rawValue = value * 100 / 10;
            mData[3]           = rawValue & 0xFF;
            mData[4]           = (rawValue >> 8) & 0xFF;
        }
        break;
        case MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE:
        {
            short int rawValue = value * 100 / 10;
            mData[7]           = rawValue & 0xFF;
            mData[8]           = (rawValue >> 8) & 0xFF;
        }
        break;
        case MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE:
        {
            short int rawValue = value * 100 / 10;
            mData[5]           = rawValue & 0xFF;
            mData[6]           = (rawValue >> 8) & 0xFF;
        }
        break;
        case MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE:
        case MAIN_ATTR_FLOAT::FAN_SPEED_PWM:
            break;
    }
}

void MainStatusMessage::setAttr(MAIN_ATTR_BOOL attr, bool value)
{
    switch (attr)
    {
        case MAIN_ATTR_BOOL::STATE_PV:
            if (value)
            {
                mData[22] |= 0x10;
            }
            else
            {
                mData[22] &= ~0x10;
            }
            break;
        case MAIN_ATTR_BOOL::STATE_SOLAR:
            if (value)
            {
                mData[22] |= 0x20;
            }
            else
            {
                mData[22] &= ~0x20;
            }
            break;
        case MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT:
        case MAIN_ATTR_BOOL::STATE_HEATPUMP:
        case MAIN_ATTR_BOOL::STATE_BOILER_BACKUP:
        case MAIN_ATTR_BOOL::STATE_FAN:
        case MAIN_ATTR_BOOL::STATE_DEFROST:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_HEAT_EXCHANGER:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING:
            break;
    }
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
        case MAIN_ATTR_FLOAT::FAN_SPEED_PWM:
            return true;
        case MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE:
            return false;
    }
    return false;
}

bool MainStatusMessage::hasAttr(MAIN_ATTR_BOOL attr) const
{
    switch (attr)
    {
        case MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT:
        case MAIN_ATTR_BOOL::STATE_HEATPUMP:
        case MAIN_ATTR_BOOL::STATE_BOILER_BACKUP:
        case MAIN_ATTR_BOOL::STATE_FAN:
        case MAIN_ATTR_BOOL::STATE_DEFROST:
        case MAIN_ATTR_BOOL::STATE_PV:
        case MAIN_ATTR_BOOL::STATE_SOLAR:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_HEAT_EXCHANGER:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION:
        case MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING:
            return true;
    }
    return false;
}

bool MainStatusMessage::hasAttr(MAIN_ATTR_U8 attr) const
{
    switch (attr)
    {
        case MAIN_ATTR_U8::ERROR_CODE:
        case MAIN_ATTR_U8::SETTING_FAN_PWM_FIRST:
        case MAIN_ATTR_U8::SETTING_FAN_PWM_SECOND:
        case MAIN_ATTR_U8::SETTING_FAN_PWM_THIRD:
        case MAIN_ATTR_U8::SETTING_MIN_TARGET_WATER_TEMPERATURE:
        case MAIN_ATTR_U8::SETTING_LEGIONELLA_TARGET_WATER_TEMPERATURE:
        case MAIN_ATTR_U8::SETTING_BRAND:
            return true;
    }
    return false;
}

bool MainStatusMessage::hasAttr(MAIN_ATTR_U16 attr) const
{
    switch (attr)
    {
        case MAIN_ATTR_U16::SETTING_HEAT_ELEMENT_WATTAGE:
        case MAIN_ATTR_U16::SETTING_BOILER_CAPACITY:
            return true;
    }
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
}  // namespace aquamqtt::message::legacy
