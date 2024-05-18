#ifndef AQUAMQTT_MAINENERGYMESSAGE_H
#define AQUAMQTT_MAINENERGYMESSAGE_H

#include <cstring>

namespace aquamqtt
{
namespace message
{
class MainEnergyMessage
{
public:
    explicit MainEnergyMessage(uint8_t* data);

    ~MainEnergyMessage() = default;

    uint32_t totalHeatpumpHours();

    uint32_t totalHeatingElemHours();

    uint32_t totalHours();

    uint64_t totalEnergyCounter();

    uint16_t powerHeatpump();

    uint16_t powerHeatElement();

    uint16_t powerOverall();

    void compareWith(uint8_t* data);

    bool totalHeatpumpHoursChanged() const;

    bool totalHeatingElemHoursChanged() const;

    bool totalHoursChanged() const;

    bool totalEnergyCounterChanged() const;

    bool powerHeatpumpChanged() const;

    bool powerHeatElementChanged() const;

    bool powerOverallChanged() const;

private:
    uint8_t* mData;

    bool mPowerHeatpumpChanged;
    bool mPowerHeatelementChanged;
    bool mPowerOverallChanged;
    bool mTotalHeatpumpHoursChanged;
    bool mTotalHeatElementHoursChanged;
    bool mTotalHoursChanged;
    bool mTotalEnergyChanged;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MAINENERGYMESSAGE_H
