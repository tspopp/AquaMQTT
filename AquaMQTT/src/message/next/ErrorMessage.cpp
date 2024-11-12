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
    // there are no empty messages in next protocol
    return false;
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