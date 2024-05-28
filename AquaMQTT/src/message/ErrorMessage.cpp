#include "message/ErrorMessage.h"

namespace aquamqtt
{
namespace message
{

ErrorMessage::ErrorMessage(uint8_t* data) : mData(data)
{
}

bool ErrorMessage::isEmpty()
{
    // i == 1, since we skip the first byte which is length field and always != 0
    for (int i = 1; i < ERROR_MESSAGE_LENGTH; ++i)
    {
        if (mData[i] != 0)
            return false;
    }
    return true;
}

uint8_t ErrorMessage::errorRequestId()
{
    return mData[1];
}

uint8_t ErrorMessage::errorCode()
{
    return mData[2];
}

float ErrorMessage::hotWaterTemp()
{
    return (float) (((short int) (mData[5] << 8) | mData[4]) / 10.0);
}

float ErrorMessage::airTemp()
{
    return (float) (((short int) (mData[7] << 8) | mData[6]) / 10.0);
}

float ErrorMessage::evaporatorLowerAirTemp()
{
    return (float) (((short int) (mData[9] << 8) | mData[8]) / 10.0);
}

float ErrorMessage::evaporatorUpperAirTemp()
{
    return (float) (((short int) (mData[11] << 8) | mData[10]) / 10.0);
}

float ErrorMessage::fanSpeedPwm()
{
    return mData[21];
}

uint16_t ErrorMessage::totalHeatpumpHours()
{
    return ((uint16_t) mData[28] << 8) | (uint16_t) mData[27];
}

uint16_t ErrorMessage::totalHeatingElemHours()
{
    return ((uint16_t) mData[26] << 8) | (uint16_t) mData[25];
}

uint8_t ErrorMessage::timeHours()
{
    return mData[32];
}
uint8_t ErrorMessage::timeMinutes()
{
    return mData[31];
}
uint16_t ErrorMessage::dateYear()
{
    return 2000 + (mData[30] / 2);
}
uint8_t ErrorMessage::dateMonth()
{
    return (mData[29] >> 5) + ((mData[30] % 2) * 8);
}
uint8_t ErrorMessage::dateDay()
{
    return mData[29] & 0x1F;
}
}  // namespace message
}  // namespace aquamqtt