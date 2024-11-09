#ifndef AQUAMQTT_IMAINMESSAGE_H
#define AQUAMQTT_IMAINMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt
{
namespace message
{

// TODO: think about a new interface
// e.g. virtual float getAttr(FLOAT_ATTR);
// e.g .virtual float setAttr(FLOAT_ATTR);
// e.g. virtual float hasAttr(FLOAT_ATTR);
// e.g. virtual float hasChanged(FLOAT_ATTR)
class IMainMessage
{
public:
    virtual ~IMainMessage() = default;

    virtual uint8_t getLength() = 0;

    virtual float hotWaterTemp() = 0;

    virtual void setHotWaterTemp(float temp) = 0;

    virtual float airTemp() = 0;

    virtual void setAirTemp(float temp) = 0;

    virtual float evaporatorLowerAirTemp() = 0;

    virtual void setEvaporatorLowerAirTemp(float temp) = 0;

    virtual float evaporatorUpperAirTemp() = 0;

    virtual void setEvaporatorUpperAirTemp(float temp) = 0;

    virtual float fanSpeedPwm() = 0;

    virtual bool stateHeatingElement() = 0;

    virtual bool stateHeatpump() = 0;

    virtual bool stateBoilerBackup() = 0;

    virtual bool stateFan() = 0;

    virtual bool stateDefrost() = 0;

    virtual bool statePV() = 0;

    virtual void enableStatePV(bool enabled) = 0;

    virtual bool stateSolar() = 0;

    virtual void enableStateSolar(bool enabled) = 0;

    virtual uint8_t settingPwmFirst() = 0;

    virtual uint8_t settingPwmSecond() = 0;

    virtual uint8_t settingPwmThird() = 0;

    virtual uint8_t settingMinTTarget() = 0;

    virtual uint8_t settingLegionellaTTarget() = 0;

    virtual uint16_t settingWattageHeatingElement() = 0;

    virtual uint16_t settingBoilerCapacity() = 0;

    virtual MAINBrands settingBrand() = 0;

    virtual bool capabilityHasHeatExchanger() = 0;

    virtual bool capabilityHasCirculation() = 0;

    virtual bool capabilityHasPVInput() = 0;

    virtual bool capabilityHasCommunication() = 0;

    virtual bool capabilityHasAntiDryHeating() = 0;

    virtual void compareWith(uint8_t* data) = 0;

    virtual bool hotWaterTempChanged() const = 0;

    virtual bool airTempChanged() const = 0;

    virtual bool evaporatorLowerAirTempChanged() const = 0;

    virtual bool evaporatorUpperAirTempChanged() const = 0;

    virtual bool fanSpeedChanged() const = 0;

    virtual bool statesChanged() const = 0;

    virtual bool statePVOrSolarChanged() const = 0;

    virtual uint8_t errorCode() const = 0;

    virtual bool settingPwmFirstChanged() const = 0;

    virtual bool settingPwmSecondChanged() const = 0;

    virtual bool settingPwmThirdChanged() const = 0;

    virtual bool settingMinTTargetChanged() const = 0;

    virtual bool settingLegionellaTTargetChanged() const = 0;

    virtual bool settingWattageHeatingElementChanged() const = 0;

    virtual bool settingBoilerCapacityChanged() const = 0;

    virtual bool settingBrandChanged() const = 0;

    virtual bool settingCapabilitiesChanged() const = 0;

    virtual bool errorCodeChanged() const = 0;

    virtual bool compressorOutletTempChanged() const = 0;

    virtual float compressorOutletTemp() = 0;

    virtual void setCompressorTemp(float d) = 0;
};
}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_IMAINMESSAGE_H
