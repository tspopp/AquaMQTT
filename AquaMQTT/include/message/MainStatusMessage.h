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

    float hotWaterTemp()
    {
        return (float) (((int16_t) mData[2] << 8) | mData[1]) / 10.0;
    }

    float airTemp()
    {
        return (float) (((int16_t) mData[4] << 8) | mData[3]) / 10.0;
    }

    float evaporatorLowerAirTemp()
    {
        return (float) (((int16_t) mData[6] << 8) | mData[5]) / 10.0;
    }

    float evaporatorUpperAirTemp()
    {
        return (float) (((int16_t) mData[8] << 8) | mData[7]) / 10.0;
    }

    uint16_t fanSpeed()
    {
        return ((uint16_t) mData[19] << 8) | (uint16_t) mData[18];
    }

    bool stateHeatingElement()
    {
        return mData[17] & 0x01;
    }

    bool stateHeatpump()
    {
        return mData[17] & 0x02;
    }
    bool stateBoilerBackup()
    {
        return mData[17] & 0x04;
    }

    bool stateFan()
    {
        return mData[17] & 0x08;
    }

    bool stateDefrost()
    {
        return mData[17] & 0x20;
    }

private:
    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MAINSTATUSMESSAGE_H
