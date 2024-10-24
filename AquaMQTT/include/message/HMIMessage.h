#ifndef AQUAMQTT_HMIMESSAGE_H
#define AQUAMQTT_HMIMESSAGE_H

#include <cstring>

#include "MessageConstants.h"

namespace aquamqtt
{
namespace message
{
class HMIMessage
{
public:
    explicit HMIMessage(uint8_t* data);

    ~HMIMessage() = default;

    float waterTempTarget();

    void setWaterTempTarget(float targetTemperature);

    HMIOperationMode operationMode() const;

    void setOperationMode(HMIOperationMode operationMode) const;

    HMIOperationType getOperationType() const;

    void setOperationType(HMIOperationType operationType) const;

    bool isEmergencyModeEnabled() const;

    void setEmergencyMode(bool enabled) const;

    bool isHeatingElementEnabled() const;

    void enableHeatingElement(bool enabled) const;

    bool isPVInputActivated() const;

    HMISetup setupMode() const;

    uint8_t antiLegionellaModePerMonth() const;

    void setAntiLegionellaModePerMonth(uint8_t value);

    HMIAirDuctConfig airDuctConfig() const;

    void setAirDuctConfig(HMIAirDuctConfig config) const;

    HMIInstallation installationMode();

    HMIFanExhaust fanExhaust() const;

    void setFanExhaustMode(HMIFanExhaust mode) const;

    bool exhaustFanChanged() const;

    HMITestMode testMode();

    void timerWindowStr(bool firstWindow, char* buffer);

    void setTimeWindowByStr(bool firstWindow, char* buffer, uint8_t length);

    uint16_t timerWindowAStart() const;

    uint16_t timerWindowALength() const;

    uint16_t timerWindowBStart() const;

    uint16_t timerWindowBLength() const;

    uint8_t timeHours() const;

    void setTimeHours(uint8_t hour) const;

    uint8_t timeMinutes() const;

    void setTimeMinutes(uint8_t minute) const;

    uint8_t timeSeconds() const;

    void setTimeSeconds(uint8_t second) const;

    uint16_t dateYear() const;

    void setDateMonthAndYear(uint8_t month, uint16_t year) const;

    uint8_t dateMonth() const;

    uint8_t dateDay() const;

    void setDateDay(uint8_t day) const;

    void setInstallationMode(HMIInstallation mode) const;

    bool waterTempTargetChanged() const;

    bool operationTypeOrModeChanged() const;

    bool timeChanged() const;

    bool dateChanged() const;

    bool emergencyModeChanged() const;

    bool heatingElemOrSetupStateOrPVActiveChanged() const;

    bool legionellaOrAirductChanged() const;

    bool testModeChanged() const;

    bool installationConfigChanged() const;

    bool timerModeOneChanged() const;

    bool timerModeTwoChanged() const;

    void compareWith(uint8_t* data);

    uint8_t errorRequestId() const;

    uint8_t errorNumberRequested() const;

    bool errorRequestChanged() const;

private:
    bool mTargetTempChanged;
    bool mOperationModeChanged;
    bool mLegionellaAirductChanged;
    bool mEmergencyModeChanged;
    bool mInstallConfigChanged;
    bool mHeatingElemOrSetupStateOrPVActiveChanged;
    bool mTimerModeOneChanged;
    bool mTimerModeTwoChanged;
    bool mTimeChanged;
    bool mDateChanged;
    bool mTestModeChanged;
    bool mErrorRequestChanged;
    bool mExhaustFanChanged;

    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_HMIMESSAGE_H
