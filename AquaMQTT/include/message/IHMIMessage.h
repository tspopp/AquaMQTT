#ifndef AQUAMQTT_IHMIMESSAGE_H
#define AQUAMQTT_IHMIMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt::message
{

enum class HMI_ATTR_U16
{
    TIMER_A_START,
    TIMER_A_LENGTH,
    TIMER_B_START,
    TIMER_B_LENGTH,
    DATE_YEAR,
};

enum class HMI_ATTR_U8
{
    ANTI_LEGIONELLA_CYCLES,
    TIME_SECONDS,
    TIME_MINUTES,
    TIME_HOURS,
    DATE_DAY,
    DATE_MONTH,
    STATE_SETUP,
    STATE_TEST,
    STATE_INSTALLATION_MODE,
    CONFIG_AIRDUCT,
    CONFIG_FAN_EXHAUST,
    HMI_ERROR_ID_REQUESTED,
    HMI_ERROR_NO_REQUESTED,
    OPERATION_MODE,
    OPERATION_TYPE,
};

enum class HMI_ATTR_FLOAT
{
    WATER_TARGET_TEMPERATURE,
};

enum class HMI_ATTR_BOOL
{
    EMERGENCY_MODE_ENABLED,
    HEATING_ELEMENT_ALLOWED,
    PV_INPUT_ALLOWED,
};

enum class HMI_ATTR_STR
{
    TIMER_WINDOW_A,
    TIMER_WINDOW_B
};

class IHMIMessage
{
public:
    virtual ~IHMIMessage() = default;

    virtual uint8_t getLength() = 0;

    virtual void setDateMonthAndYear(uint8_t month, uint16_t year) const = 0;

    virtual uint8_t  getAttr(HMI_ATTR_U8 attr)                = 0;
    virtual uint16_t getAttr(HMI_ATTR_U16 attr)               = 0;
    virtual bool     getAttr(HMI_ATTR_BOOL attr)              = 0;
    virtual float    getAttr(HMI_ATTR_FLOAT attr)             = 0;
    virtual void     getAttr(HMI_ATTR_STR attr, char* buffer) = 0;

    virtual void setAttr(HMI_ATTR_U8 attr, uint8_t value)   = 0;
    virtual void setAttr(HMI_ATTR_U16 attr, uint16_t value) = 0;
    virtual void setAttr(HMI_ATTR_BOOL attr, bool value)    = 0;
    virtual void setAttr(HMI_ATTR_FLOAT attr, float value)  = 0;

    virtual bool hasAttr(HMI_ATTR_U8 attr) const    = 0;
    virtual bool hasAttr(HMI_ATTR_U16 attr) const   = 0;
    virtual bool hasAttr(HMI_ATTR_BOOL attr) const  = 0;
    virtual bool hasAttr(HMI_ATTR_FLOAT attr) const = 0;
    virtual bool hasAttr(HMI_ATTR_STR attr) const   = 0;

    virtual bool hasChanged(HMI_ATTR_U8 attr) const    = 0;
    virtual bool hasChanged(HMI_ATTR_U16 attr) const   = 0;
    virtual bool hasChanged(HMI_ATTR_BOOL attr) const  = 0;
    virtual bool hasChanged(HMI_ATTR_FLOAT attr) const = 0;
    virtual bool hasChanged(HMI_ATTR_STR attr) const   = 0;
};
}  // namespace aquamqtt::message

#endif  // AQUAMQTT_IHMIMESSAGE_H
