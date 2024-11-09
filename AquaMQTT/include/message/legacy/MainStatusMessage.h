#ifndef AQUAMQTT_LEGACYMAINSTATUSMESSAGE_H
#define AQUAMQTT_LEGACYMAINSTATUSMESSAGE_H

#include <cstring>
#include <memory>

#include "message/IMainMessage.h"

namespace aquamqtt
{
namespace message
{
namespace legacy
{
class MainStatusMessage : public IMainMessage
{
public:
    explicit MainStatusMessage(uint8_t* data);

    ~MainStatusMessage() override = default;

    uint8_t getLength() override;

    float hotWaterTemp() override;

    void setHotWaterTemp(float temp) override;

    float airTemp() override;

    void setAirTemp(float temp) override;

    float evaporatorLowerAirTemp() override;

    void setEvaporatorLowerAirTemp(float temp) override;

    float evaporatorUpperAirTemp() override;

    void setEvaporatorUpperAirTemp(float temp) override;

    float fanSpeedPwm() override;

    bool stateHeatingElement() override;

    bool stateHeatpump() override;

    bool stateBoilerBackup() override;

    bool stateFan() override;

    bool stateDefrost() override;

    bool statePV() override;

    void enableStatePV(bool enabled) override;

    bool stateSolar() override;

    void enableStateSolar(bool enabled) override;

    uint8_t settingPwmFirst() override;

    uint8_t settingPwmSecond() override;

    uint8_t settingPwmThird() override;

    uint8_t settingMinTTarget() override;

    uint8_t settingLegionellaTTarget() override;

    uint16_t settingWattageHeatingElement() override;

    uint16_t settingBoilerCapacity() override;

    MAINBrands settingBrand() override;

    bool capabilityHasHeatExchanger() override;

    bool capabilityHasCirculation() override;

    bool capabilityHasPVInput() override;

    bool capabilityHasCommunication() override;

    bool capabilityHasAntiDryHeating() override;

    void compareWith(uint8_t* data) override;

    bool hotWaterTempChanged() const override;

    bool airTempChanged() const override;

    bool evaporatorLowerAirTempChanged() const override;

    bool evaporatorUpperAirTempChanged() const override;

    bool fanSpeedChanged() const override;

    bool statesChanged() const override;

    bool statePVOrSolarChanged() const override;

    uint8_t errorCode() const override;

    bool settingPwmFirstChanged() const override;

    bool settingPwmSecondChanged() const override;

    bool settingPwmThirdChanged() const override;

    bool settingMinTTargetChanged() const override;

    bool settingLegionellaTTargetChanged() const override;

    bool settingWattageHeatingElementChanged() const override;

    bool settingBoilerCapacityChanged() const override;

    bool settingBrandChanged() const override;

    bool settingCapabilitiesChanged() const override;

    bool errorCodeChanged() const override;

    bool compressorOutletTempChanged() const override;

    float compressorOutletTemp() override;

    void setCompressorTemp(float d) override;

private:
    uint8_t* mData;
    bool     mHotWaterTempChanged;
    bool     mAirTempChanged;
    bool     mEvaporatorLowerChanged;
    bool     mEvaporatorUpperChanged;
    bool     mFanSpeedChanged;
    bool     mStatesChanged;
    bool     mSettingMinTChanged;
    bool     mSettingPwmFirstChanged;
    bool     mSettingPwmSecondChanged;
    bool     mSettingPwmThirdChanged;
    bool     mSettingAntiLegionellaTargetChanged;
    bool     mSettingWattageHeatElementChanged;
    bool     mSettingBoilerCapacityChanged;
    bool     mSettingBoilerBrandChanged;
    bool     mSettingCapabilitiesChanged;
    bool     mPVOrSolarStateChanged;
    bool     mErrorCodeChanged;
    bool     mCompresserOutletTempChanged;
};

}  // namespace next
}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_LEGACYMAINSTATUSMESSAGE_H
