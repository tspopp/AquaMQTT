#include "message/legacy/ErrorMessage.h"

namespace aquamqtt::message::legacy
{

ErrorMessage::ErrorMessage(uint8_t* data) : mData(data)
{
}

bool ErrorMessage::isEmpty()
{
    // i == 1, since we skip the first byte which is length field and always != 0
    for (int i = 1; i < ERROR_MESSAGE_LENGTH_LEGACY; ++i)
    {
        if (mData[i] != 0)
            return false;
    }
    return true;
}

uint8_t ErrorMessage::getLength()
{
    return ERROR_MESSAGE_LENGTH_LEGACY;
}

uint8_t ErrorMessage::getAttr(ERROR_ATTR_U8 attr)
{
    switch (attr)
    {
        case ERROR_ATTR_U8::ERROR_REQUEST_ID:
            return mData[1];
        case ERROR_ATTR_U8::ERROR_ERROR_CODE:
            return mData[2];
        case ERROR_ATTR_U8::ERROR_TIME_MINUTES:
            return mData[31];
        case ERROR_ATTR_U8::ERROR_TIME_HOURS:
            return mData[32];
        case ERROR_ATTR_U8::ERROR_DATE_DAY:
            return mData[29] & 0x1F;
        case ERROR_ATTR_U8::ERROR_DATE_MONTH:
            return (mData[29] >> 5) + ((mData[30] % 2) * 8);
        case ERROR_ATTR_U8::ERROR_DATE_YEAR:
            return 2000 + (mData[30] / 2);
    }
    return 0;
}

uint16_t ErrorMessage::getAttr(ERROR_ATTR_U16 attr)
{
    switch (attr)
    {

        case ERROR_ATTR_U16::ERROR_TOTAL_HEATPUMP_HOURS:
            return ((uint16_t) mData[28] << 8) | (uint16_t) mData[27];
        case ERROR_ATTR_U16::ERROR_TOTAL_HEATING_ELEMENT_HOURS:
            return ((uint16_t) mData[26] << 8) | (uint16_t) mData[25];
    }
    return 0;
}

float ErrorMessage::getAttr(ERROR_ATTR_FLOAT attr)
{
    switch (attr)
    {

        case ERROR_ATTR_FLOAT::ERROR_WATER_TEMPERATURE:
            return (float) (((short int) (mData[5] << 8) | mData[4]) / 10.0);
        case ERROR_ATTR_FLOAT::ERROR_AIR_TEMPERATURE:
            return (float) (((short int) (mData[7] << 8) | mData[6]) / 10.0);
        case ERROR_ATTR_FLOAT::ERROR_EVAPORATOR_UPPER_TEMPERATURE:
            return (float) (((short int) (mData[11] << 8) | mData[10]) / 10.0);
        case ERROR_ATTR_FLOAT::ERROR_EVAPORATOR_LOWER_TEMPERATURE:
            return (float) (((short int) (mData[9] << 8) | mData[8]) / 10.0);
        case ERROR_ATTR_FLOAT::ERROR_COMPRESSOR_OUTLET_TEMPERATURE:
            break;
        case ERROR_ATTR_FLOAT::ERROR_FAN_SPEED_PWM:
            return mData[21];
    }
    return 0;
}

bool ErrorMessage::hasAttr(ERROR_ATTR_U8 attr) const
{
    switch (attr)
    {
        case ERROR_ATTR_U8::ERROR_REQUEST_ID:
        case ERROR_ATTR_U8::ERROR_ERROR_CODE:
        case ERROR_ATTR_U8::ERROR_TIME_MINUTES:
        case ERROR_ATTR_U8::ERROR_TIME_HOURS:
        case ERROR_ATTR_U8::ERROR_DATE_DAY:
        case ERROR_ATTR_U8::ERROR_DATE_MONTH:
        case ERROR_ATTR_U8::ERROR_DATE_YEAR:
            return true;
    }
    return false;
}

bool ErrorMessage::hasAttr(ERROR_ATTR_U16 attr) const
{
    switch (attr)
    {
        case ERROR_ATTR_U16::ERROR_TOTAL_HEATPUMP_HOURS:
        case ERROR_ATTR_U16::ERROR_TOTAL_HEATING_ELEMENT_HOURS:
            return true;
    }
    return false;
}

bool ErrorMessage::hasAttr(ERROR_ATTR_FLOAT attr) const
{
    switch (attr)
    {
        case ERROR_ATTR_FLOAT::ERROR_WATER_TEMPERATURE:
        case ERROR_ATTR_FLOAT::ERROR_AIR_TEMPERATURE:
        case ERROR_ATTR_FLOAT::ERROR_EVAPORATOR_UPPER_TEMPERATURE:
        case ERROR_ATTR_FLOAT::ERROR_EVAPORATOR_LOWER_TEMPERATURE:
        case ERROR_ATTR_FLOAT::ERROR_COMPRESSOR_OUTLET_TEMPERATURE:
        case ERROR_ATTR_FLOAT::ERROR_FAN_SPEED_PWM:
            return true;
    }
    return false;
}
}  // namespace aquamqtt::message::legacy
