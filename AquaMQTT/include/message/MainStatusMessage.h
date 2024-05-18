#ifndef AQUAMQTT_MAINSTATUSMESSAGE_H
#define AQUAMQTT_MAINSTATUSMESSAGE_H

#include <cstring>
#include <memory>

#include "MessageConstants.h"

namespace aquamqtt
{
namespace message
{
class MainStatusMessage
{
public:
    explicit MainStatusMessage(uint8_t* data);

    ~MainStatusMessage() = default;

    float hotWaterTemp();

    float airTemp();

    float evaporatorLowerAirTemp();

    float evaporatorUpperAirTemp();

    uint16_t fanSpeed();

    bool stateHeatingElement();

    bool stateHeatpump();

    bool stateBoilerBackup();

    bool stateFan();

    bool stateDefrost();

    uint8_t settingPwmFirst();

    uint8_t settingPwmSecond();

    uint8_t settingPwmThird();

    uint8_t settingMinTTarget();

    uint8_t settingLegionellaTTarget();

    uint16_t settingWattageHeatingElement();

    uint16_t settingBoilerCapacity();

    MAINBrands settingBrand();

    bool settingHasHeatExchanger();

    bool settingHasCirculation();

    bool settingHasPVInput();

    bool settingHasCommunication();

    bool settingHasAntiTrockenheizung();

    void compareWith(uint8_t* data);

    bool hotWaterTempChanged() const;

    bool airTempChanged() const;

    bool evaporatorLowerAirTempChanged() const;

    bool evaporatorUpperAirTempChanged() const;

    bool fanSpeedChanged() const;

    bool statesChanged() const;

    bool settingPwmFirstChanged() const;

    bool settingPwmSecondChanged() const;

    bool settingPwmThirdChanged() const;

    bool settingMinTTargetChanged() const;

    bool settingLegionellaTTargetChanged() const;

    bool settingWattageHeatingElementChanged() const;

    bool settingBoilerCapacityChanged() const;

    bool settingBrandChanged() const;

    bool settingCapabilitiesChanged() const;

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
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MAINSTATUSMESSAGE_H
