#ifndef AQUAMQTT_IHMIMESSAGE_H
#define AQUAMQTT_IHMIMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt
{
namespace message
{

class IHMIMessage
{
public:
    virtual ~IHMIMessage() = default;

    virtual uint8_t getLength() = 0;

    virtual float waterTempTarget() = 0;

    virtual void setWaterTempTarget(float targetTemperature) = 0;

    virtual HMIOperationMode operationMode() const = 0;

    virtual void setOperationMode(HMIOperationMode operationMode) const = 0;

    virtual HMIOperationType getOperationType() const = 0;

    virtual void setOperationType(HMIOperationType operationType) const = 0;

    virtual bool isEmergencyModeEnabled() const = 0;

    virtual void setEmergencyMode(bool enabled) const = 0;

    virtual bool isHeatingElementEnabled() const = 0;

    virtual void enableHeatingElement(bool enabled) const = 0;

    virtual bool isPVInputActivated() const = 0;

    virtual HMISetup setupMode() const = 0;

    virtual uint8_t antiLegionellaModePerMonth() const = 0;

    virtual void setAntiLegionellaModePerMonth(uint8_t value) = 0;

    virtual HMIAirDuctConfig airDuctConfig() const = 0;

    virtual void setAirDuctConfig(HMIAirDuctConfig config) const = 0;

    virtual HMIInstallation installationMode() = 0;

    virtual HMIFanExhaust fanExhaust() const = 0;

    virtual void setFanExhaustMode(HMIFanExhaust mode) const = 0;

    virtual bool exhaustFanChanged() const = 0;

    virtual HMITestMode testMode() = 0;

    virtual void timerWindowStr(bool firstWindow, char* buffer) = 0;

    virtual void setTimeWindowByStr(bool firstWindow, char* buffer, uint8_t length) = 0;

    virtual uint16_t timerWindowAStart() const = 0;

    virtual uint16_t timerWindowALength() const = 0;

    virtual uint16_t timerWindowBStart() const = 0;

    virtual uint16_t timerWindowBLength() const = 0;

    virtual uint8_t timeHours() const = 0;

    virtual void setTimeHours(uint8_t hour) const = 0;

    virtual uint8_t timeMinutes() const = 0;

    virtual void setTimeMinutes(uint8_t minute) const = 0;

    virtual uint8_t timeSeconds() const = 0;

    virtual void setTimeSeconds(uint8_t second) const = 0;

    virtual uint16_t dateYear() const = 0;

    virtual void setDateMonthAndYear(uint8_t month, uint16_t year) const = 0;

    virtual uint8_t dateMonth() const = 0;

    virtual uint8_t dateDay() const = 0;

    virtual void setDateDay(uint8_t day) const = 0;

    virtual void setInstallationMode(HMIInstallation mode) const = 0;

    virtual bool waterTempTargetChanged() const = 0;

    virtual bool operationTypeOrModeChanged() const = 0;

    virtual bool timeChanged() const = 0;

    virtual bool dateChanged() const = 0;

    virtual bool emergencyModeChanged() const = 0;

    virtual bool heatingElemOrSetupStateOrPVActiveChanged() const = 0;

    virtual bool legionellaOrAirductChanged() const = 0;

    virtual bool testModeChanged() const = 0;

    virtual bool installationConfigChanged() const = 0;

    virtual bool timerModeOneChanged() const = 0;

    virtual bool timerModeTwoChanged() const = 0;

    virtual void compareWith(uint8_t* data) = 0;

    virtual uint8_t errorRequestId() const = 0;

    virtual uint8_t errorNumberRequested() const = 0;

    virtual bool errorRequestChanged() const = 0;
};
}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_IHMIMESSAGE_H
