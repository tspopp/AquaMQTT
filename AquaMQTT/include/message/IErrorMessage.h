#ifndef AQUAMQTT_IERRORMESSAGE_H
#define AQUAMQTT_IERRORMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt::message
{

enum class ERROR_ATTR_U8
{
    ERROR_REQUEST_ID,
    ERROR_ERROR_CODE,
    ERROR_TIME_MINUTES,
    ERROR_TIME_HOURS,
    ERROR_DATE_DAY,
    ERROR_DATE_MONTH,
};

enum class ERROR_ATTR_FLOAT
{
    ERROR_WATER_TEMPERATURE,
    ERROR_AIR_TEMPERATURE,
    ERROR_EVAPORATOR_UPPER_TEMPERATURE,
    ERROR_EVAPORATOR_LOWER_TEMPERATURE,
    ERROR_COMPRESSOR_OUTLET_TEMPERATURE,
    ERROR_FAN_SPEED_PWM,
};

enum class ERROR_ATTR_U16
{
    ERROR_TOTAL_HEATPUMP_HOURS,
    ERROR_TOTAL_HEATING_ELEMENT_HOURS,
    ERROR_DATE_YEAR
};

class IErrorMessage
{
public:
    virtual ~IErrorMessage() = default;

    virtual uint8_t getLength() = 0;

    virtual bool isEmpty() = 0;

    virtual uint8_t  getAttr(ERROR_ATTR_U8 attr)    = 0;
    virtual uint16_t getAttr(ERROR_ATTR_U16 attr)   = 0;
    virtual float    getAttr(ERROR_ATTR_FLOAT attr) = 0;

    virtual bool hasAttr(ERROR_ATTR_U8 attr) const    = 0;
    virtual bool hasAttr(ERROR_ATTR_U16 attr) const   = 0;
    virtual bool hasAttr(ERROR_ATTR_FLOAT attr) const = 0;
};
}  // namespace aquamqtt::message

#endif  // AQUAMQTT_IERRORMESSAGE_H
