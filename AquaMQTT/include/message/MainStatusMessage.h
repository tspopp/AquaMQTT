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
    explicit MainStatusMessage(uint8_t* data) : mData(data)
    {
    }

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

private:
    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MAINSTATUSMESSAGE_H
