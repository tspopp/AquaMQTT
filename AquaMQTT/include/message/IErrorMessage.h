#ifndef AQUAMQTT_IERRORMESSAGE_H
#define AQUAMQTT_IERRORMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt
{
namespace message
{

class IErrorMessage
{
public:
    virtual ~IErrorMessage()  = default;

    virtual uint8_t getLength() = 0;

    virtual uint8_t errorRequestId() = 0;

    virtual uint8_t errorCode() = 0;

    virtual float hotWaterTemp() = 0;

    virtual float airTemp() = 0;

    virtual float evaporatorLowerAirTemp() = 0;

    virtual float evaporatorUpperAirTemp() = 0;

    virtual float fanSpeedPwm() = 0;

    virtual uint16_t totalHeatpumpHours() = 0;

    virtual uint16_t totalHeatingElemHours() = 0;

    virtual uint8_t timeHours() = 0;

    virtual uint8_t timeMinutes() = 0;

    virtual uint16_t dateYear() = 0;

    virtual uint8_t dateMonth()  = 0;

    virtual uint8_t dateDay()  = 0;

    virtual bool isEmpty()  = 0;

};
}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_IERRORMESSAGE_H
