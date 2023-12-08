#include "message/MainStatusMessage.h"

namespace aquamqtt
{
namespace message
{

float message::MainStatusMessage::hotWaterTemp()
{
    return (float) (((int16_t) mData[2] << 8) | mData[1]) / 10.0;
}
float MainStatusMessage::airTemp()
{
    return (float) (((int16_t) mData[4] << 8) | mData[3]) / 10.0;
}
float MainStatusMessage::evaporatorLowerAirTemp()
{
    return (float) (((int16_t) mData[6] << 8) | mData[5]) / 10.0;
}
float MainStatusMessage::evaporatorUpperAirTemp()
{
    return (float) (((int16_t) mData[8] << 8) | mData[7]) / 10.0;
}
uint16_t MainStatusMessage::fanSpeed()
{
    return ((uint16_t) mData[19] << 8) | (uint16_t) mData[18];
}
bool MainStatusMessage::stateHeatingElement()
{
    return mData[17] & 0x01;
}
bool MainStatusMessage::stateHeatpump()
{
    return mData[17] & 0x02;
}
bool MainStatusMessage::stateBoilerBackup()
{
    return mData[17] & 0x04;
}
bool MainStatusMessage::stateFan()
{
    return mData[17] & 0x08;
}
bool MainStatusMessage::stateDefrost()
{
    return mData[17] & 0x20;
}
}
}  // namespace aquamqtt