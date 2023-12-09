#ifndef AQUAMQTT_MAINSTATUSMESSAGE_H
#define AQUAMQTT_MAINSTATUSMESSAGE_H

#include <cstring>
#include <memory>

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

private:
    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MAINSTATUSMESSAGE_H
