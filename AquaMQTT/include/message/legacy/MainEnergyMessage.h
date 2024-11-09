#ifndef AQUAMQTT_LEGACYMAINENERGYMESSAGE_H
#define AQUAMQTT_LEGACYMAINENERGYMESSAGE_H

#include <cstring>

#include "message/IEnergyMessage.h"

namespace aquamqtt
{
namespace message
{
namespace legacy
{
class MainEnergyMessage : public IEnergyMessage
{
public:
    explicit MainEnergyMessage(uint8_t* data);

    ~MainEnergyMessage() override = default;

    uint8_t getLength() override;

    uint32_t totalHeatpumpHours() override;

    uint32_t totalHeatingElemHours() override;

    uint32_t totalHours() override;

    uint64_t totalEnergyCounter() override;

    uint16_t powerHeatpump() override;

    uint16_t powerHeatElement() override;

    uint16_t powerOverall() override;

    uint16_t totalWaterProduction() override;

    void compareWith(uint8_t* data) override;

    bool totalHeatpumpHoursChanged() const override;

    bool totalHeatingElemHoursChanged() const override;

    bool totalHoursChanged() const override;

    bool totalEnergyCounterChanged() const override;

    bool powerHeatpumpChanged() const override;

    bool powerHeatElementChanged() const override;

    bool powerOverallChanged() const override;

    bool totalWaterProductionChanged() const override;

private:
    uint8_t* mData;

    bool mPowerHeatpumpChanged;
    bool mPowerHeatelementChanged;
    bool mPowerOverallChanged;
    bool mTotalHeatpumpHoursChanged;
    bool mTotalHeatElementHoursChanged;
    bool mTotalHoursChanged;
    bool mTotalEnergyChanged;
    bool mTotalWaterProductionChanged;
};

}  // namespace next
}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_LEGACYMAINENERGYMESSAGE_H
