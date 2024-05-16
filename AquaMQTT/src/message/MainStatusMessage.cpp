#include "message/MainStatusMessage.h"

namespace aquamqtt
{
namespace message
{

float message::MainStatusMessage::hotWaterTemp()
{
    return (float) (((short int) (mData[2] << 8) | mData[1]) / 10.0);
}
float MainStatusMessage::airTemp()
{
    return (float) (((short int) (mData[4] << 8) | mData[3]) / 10.0);
}
float MainStatusMessage::evaporatorLowerAirTemp()
{
    return (float) (((short int) (mData[6] << 8) | mData[5]) / 10.0);
}
float MainStatusMessage::evaporatorUpperAirTemp()
{
    return (float) (((short int) (mData[8] << 8) | mData[7]) / 10.0);
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
uint8_t MainStatusMessage::settingMinTTarget()
{
    return mData[20];
}
uint8_t MainStatusMessage::settingPwmFirst()
{
    return mData[15];
}
uint8_t MainStatusMessage::settingPwmSecond()
{
    return mData[16];
}
uint8_t MainStatusMessage::settingPwmThird()
{
    return mData[14];
}
uint8_t MainStatusMessage::settingLegionellaTTarget()
{
    return mData[21];
}
uint16_t MainStatusMessage::settingWattageHeatingElement()
{
    return mData[32] * 100;
}
uint16_t MainStatusMessage::settingBoilerCapacity()
{
    return ((uint16_t) mData[34] << 8) | (uint16_t) mData[33];
}
MAINBrands MainStatusMessage::settingBrand()
{
    switch (mData[35])
    {
        case 65:
            return MAINBrands::BR_ATLANTIC;
        case 78:
            return MAINBrands::BR_NONAME;
        case 83:
            return MAINBrands::BR_SAUTER;
        case 84:
            return MAINBrands::BR_THERMOR;
        default:
            return MAINBrands::BR_UNKNOWN;
    }
}
bool MainStatusMessage::settingHasHeatExchanger()
{
    return mData[36] & 0x01;
}
bool MainStatusMessage::settingHasCirculation()
{
    return mData[36] & 0x02;
}
bool MainStatusMessage::settingHasPVInput()
{
    return mData[36] & 0x04;
}
bool MainStatusMessage::settingHasCommunication()
{
    return !(mData[36] & 0x08);
}
bool MainStatusMessage::settingHasAntiTrockenheizung()
{
    return !(mData[36] & 0x20);
}
}  // namespace message
}  // namespace aquamqtt