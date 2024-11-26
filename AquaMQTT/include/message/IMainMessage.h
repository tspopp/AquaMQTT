#ifndef AQUAMQTT_IMAINMESSAGE_H
#define AQUAMQTT_IMAINMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt::message
{

enum class MAIN_ATTR_FLOAT
{
    WATER_TEMPERATURE,
    AIR_TEMPERATURE,
    EVAPORATOR_UPPER_TEMPERATURE,
    EVAPORATOR_LOWER_TEMPERATURE,
    COMPRESSOR_OUTLET_TEMPERATURE,
    FAN_SPEED_PWM,
};

enum class MAIN_ATTR_BOOL
{
    STATE_HEATING_ELEMENT,
    STATE_HEATPUMP,
    STATE_BOILER_BACKUP,
    STATE_FAN,
    STATE_DEFROST,
    STATE_PV,
    STATE_SOLAR,
    CAPABILITY_HAS_HEAT_EXCHANGER,
    CAPABILITY_HAS_CIRCULATION,
    CAPABILITY_HAS_PV_INPUT,
    CAPABILITY_HAS_COMMUNICATION,
    CAPABILITY_HAS_ANTI_DRY_HEATING,
};

enum class MAIN_ATTR_U8
{
    ERROR_CODE,
    SETTING_FAN_PWM_FIRST,
    SETTING_FAN_PWM_SECOND,
    SETTING_FAN_PWM_THIRD,
    SETTING_MIN_TARGET_WATER_TEMPERATURE,
    SETTING_LEGIONELLA_TARGET_WATER_TEMPERATURE,
    SETTING_BRAND
};

enum class MAIN_ATTR_U16
{
    SETTING_HEAT_ELEMENT_WATTAGE,
    SETTING_BOILER_CAPACITY
};

class IMainMessage
{
public:
    virtual ~IMainMessage() = default;

    virtual uint8_t getLength() = 0;

    virtual float    getAttr(MAIN_ATTR_FLOAT attr) = 0;
    virtual bool     getAttr(MAIN_ATTR_BOOL attr)  = 0;
    virtual uint8_t  getAttr(MAIN_ATTR_U8 attr)    = 0;
    virtual uint16_t getAttr(MAIN_ATTR_U16 attr)   = 0;

    virtual void setAttr(MAIN_ATTR_FLOAT attr, float value)  = 0;
    virtual void setAttr(MAIN_ATTR_BOOL attr, bool value)    = 0;
    virtual void setAttr(MAIN_ATTR_U8 attr, uint8_t value)   = 0;
    virtual void setAttr(MAIN_ATTR_U16 attr, uint16_t value) = 0;

    virtual bool hasAttr(MAIN_ATTR_FLOAT attr) const = 0;
    virtual bool hasAttr(MAIN_ATTR_BOOL attr) const  = 0;
    virtual bool hasAttr(MAIN_ATTR_U8 attr) const    = 0;
    virtual bool hasAttr(MAIN_ATTR_U16 attr) const   = 0;

    virtual bool hasChanged(MAIN_ATTR_FLOAT attr) const = 0;
    virtual bool hasChanged(MAIN_ATTR_BOOL attr) const  = 0;
    virtual bool hasChanged(MAIN_ATTR_U8 attr) const    = 0;
    virtual bool hasChanged(MAIN_ATTR_U16 attr) const   = 0;
};
}  // namespace aquamqtt::message

#endif  // AQUAMQTT_IMAINMESSAGE_H
