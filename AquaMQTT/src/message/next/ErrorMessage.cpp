#include "message/next/ErrorMessage.h"

namespace aquamqtt
{
namespace message
{
namespace next
{

ErrorMessage::ErrorMessage(uint8_t* data) : mData(data)
{
}

bool ErrorMessage::isEmpty()
{
    // TODO
    //    // i == 1, since we skip the first byte which is length field and always != 0
    //    for (int i = 1; i < ERROR_MESSAGE_LENGTH; ++i)
    //    {
    //        if (mData[i] != 0)
    //            return false;
    //    }
    return true;
}

uint8_t ErrorMessage::errorRequestId()
{
    // TODO
    return 0;
}

uint8_t ErrorMessage::errorCode()
{
    // TODO
    return 0;
}

float ErrorMessage::hotWaterTemp()
{
    // TODO
    return 0;
}

float ErrorMessage::airTemp()
{
    // TODO
    return 0;
}

float ErrorMessage::evaporatorLowerAirTemp()
{
    // TODO
    return 0;
}

float ErrorMessage::evaporatorUpperAirTemp()
{
    // TODO
    return 0;
}

float ErrorMessage::fanSpeedPwm()
{
    // TODO
    return 0;
}

uint16_t ErrorMessage::totalHeatpumpHours()
{
    // TODO
    return 0;
}

uint16_t ErrorMessage::totalHeatingElemHours()
{
    // TODO
    return 0;
}

uint8_t ErrorMessage::timeHours()
{
    // TODO
    return 0;
}
uint8_t ErrorMessage::timeMinutes()
{
    // TODO
    return 0;
}
uint16_t ErrorMessage::dateYear()
{
    // TODO
    return 0;
}
uint8_t ErrorMessage::dateMonth()
{
    // TODO
    return 0;
}
uint8_t ErrorMessage::dateDay()
{
    // TODO
    return 0;
}
uint8_t ErrorMessage::getLength()
{
    return ERROR_MESSAGE_LENGTH_NEXT;
}
}
}  // namespace message
}  // namespace aquamqtt