#ifndef AQUAMQTT_LEGACYHMIMESSAGE_H
#define AQUAMQTT_LEGACYHMIMESSAGE_H

#include <cstring>

#include "message/IHMIMessage.h"

namespace aquamqtt
{
namespace message
{
namespace legacy
{
class HMIMessage : public IHMIMessage
{
public:
    explicit HMIMessage(uint8_t* data);

    ~HMIMessage() override = default;

    uint8_t getLength() override;

    float waterTempTarget() override;

    void setWaterTempTarget(float targetTemperature) override;

    HMIOperationMode operationMode() const override;

    void setOperationMode(HMIOperationMode operationMode) const override;

    HMIOperationType getOperationType() const override;

    void setOperationType(HMIOperationType operationType) const override;

    bool isEmergencyModeEnabled() const override;

    void setEmergencyMode(bool enabled) const override;

    bool isHeatingElementEnabled() const override;

    void enableHeatingElement(bool enabled) const override;

    bool isPVInputActivated() const override;

    HMISetup setupMode() const override;

    uint8_t antiLegionellaModePerMonth() const override;

    void setAntiLegionellaModePerMonth(uint8_t value) override;

    HMIAirDuctConfig airDuctConfig() const override;

    void setAirDuctConfig(HMIAirDuctConfig config) const override;

    HMIInstallation installationMode() override;

    HMIFanExhaust fanExhaust() const override;

    void setFanExhaustMode(HMIFanExhaust mode) const override;

    bool exhaustFanChanged() const override;

    HMITestMode testMode() override;

    void timerWindowStr(bool firstWindow, char* buffer) override;

    void setTimeWindowByStr(bool firstWindow, char* buffer, uint8_t length) override;

    uint16_t timerWindowAStart() const override;

    uint16_t timerWindowALength() const override;

    uint16_t timerWindowBStart() const override;

    uint16_t timerWindowBLength() const override;

    uint8_t timeHours() const override;

    void setTimeHours(uint8_t hour) const override;

    uint8_t timeMinutes() const override;

    void setTimeMinutes(uint8_t minute) const override;

    uint8_t timeSeconds() const override;

    void setTimeSeconds(uint8_t second) const override;

    uint16_t dateYear() const override;

    void setDateMonthAndYear(uint8_t month, uint16_t year) const override;

    uint8_t dateMonth() const override;

    uint8_t dateDay() const override;

    void setDateDay(uint8_t day) const override;

    void setInstallationMode(HMIInstallation mode) const override;

    bool waterTempTargetChanged() const override;

    bool operationTypeOrModeChanged() const override;

    bool timeChanged() const override;

    bool dateChanged() const override;

    bool emergencyModeChanged() const override;

    bool heatingElemOrSetupStateOrPVActiveChanged() const override;

    bool legionellaOrAirductChanged() const override;

    bool testModeChanged() const override;

    bool installationConfigChanged() const override;

    bool timerModeOneChanged() const override;

    bool timerModeTwoChanged() const override;

    void compareWith(uint8_t* data) override;

    uint8_t errorRequestId() const override;

    uint8_t errorNumberRequested() const override;

    bool errorRequestChanged() const override;

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
}  // namespace next
}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_LEGACYHMIMESSAGE_H
