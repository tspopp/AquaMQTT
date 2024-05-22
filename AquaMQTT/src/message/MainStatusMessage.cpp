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

bool MainStatusMessage::statePV()
{
    return mData[22] & 0x20;
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

bool MainStatusMessage::capabilityHasHeatExchanger()
{
    return mData[36] & 0x01;
}
bool MainStatusMessage::capabilityHasCirculation()
{
    return mData[36] & 0x02;
}
bool MainStatusMessage::capabilityHasPVInput()
{
    return mData[36] & 0x04;
}
bool MainStatusMessage::capabilityHasCommunication()
{
    return !(mData[36] & 0x08);
}
bool MainStatusMessage::capabilityHasAntiDryHeating()
{
    return (mData[36] & 0x20);
}
void MainStatusMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        mHotWaterTempChanged                = true;
        mAirTempChanged                     = true;
        mEvaporatorLowerChanged             = true;
        mEvaporatorUpperChanged             = true;
        mFanSpeedChanged                    = true;
        mStatesChanged                      = true;
        mSettingMinTChanged                 = true;
        mSettingPwmFirstChanged             = true;
        mSettingPwmSecondChanged            = true;
        mSettingPwmThirdChanged             = true;
        mSettingAntiLegionellaTargetChanged = true;
        mSettingWattageHeatElementChanged   = true;
        mSettingBoilerCapacityChanged       = true;
        mSettingBoilerBrandChanged          = true;
        mSettingCapabilitiesChanged         = true;
        mPVStateChanged                     = true;
        return;
    }

    uint8_t diffIndices[MAIN_MESSAGE_LENGTH] = { 0 };
    size_t  numDiffs                         = 0;
    compareBuffers(mData, data, MAIN_MESSAGE_LENGTH, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 1:
            case 2:
                mHotWaterTempChanged = true;
                break;
            case 3:
            case 4:
                mAirTempChanged = true;
                break;
            case 5:
            case 6:
                mEvaporatorLowerChanged = true;
                break;
            case 7:
            case 8:
                mEvaporatorUpperChanged = true;
                break;
            case 14:
                mSettingPwmThirdChanged = true;
                break;
            case 15:
                mSettingPwmFirstChanged = true;
                break;
            case 16:
                mSettingPwmSecondChanged = true;
                break;
            case 17:
                mStatesChanged = true;
                break;
            case 18:
            case 19:
                mFanSpeedChanged = true;
                break;
            case 20:
                mSettingMinTChanged = true;
                break;
            case 21:
                mSettingAntiLegionellaTargetChanged = true;
                break;
            case 22:
                mPVStateChanged = true;
                break;
            case 32:
                mSettingWattageHeatElementChanged = true;
                break;
            case 33:
            case 34:
                mSettingBoilerCapacityChanged = true;
                break;
            case 35:
                mSettingBoilerBrandChanged = true;
                break;
            case 36:
                mSettingCapabilitiesChanged = true;
                break;
            default:
                break;
        }
    }
}
MainStatusMessage::MainStatusMessage(uint8_t* data)
    : mData(data)
    , mHotWaterTempChanged(false)
    , mAirTempChanged(false)
    , mEvaporatorLowerChanged(false)
    , mEvaporatorUpperChanged(false)
    , mFanSpeedChanged(false)
    , mStatesChanged(false)
    , mSettingMinTChanged(false)
    , mSettingPwmFirstChanged(false)
    , mSettingPwmSecondChanged(false)
    , mSettingPwmThirdChanged(false)
    , mSettingAntiLegionellaTargetChanged(false)
    , mSettingWattageHeatElementChanged(false)
    , mSettingBoilerCapacityChanged(false)
    , mSettingBoilerBrandChanged(false)
    , mSettingCapabilitiesChanged(false)
    , mPVStateChanged(false)
{
}
bool MainStatusMessage::hotWaterTempChanged() const
{
    return mHotWaterTempChanged;
}
bool MainStatusMessage::airTempChanged() const
{
    return mAirTempChanged;
}
bool MainStatusMessage::evaporatorLowerAirTempChanged() const
{
    return mEvaporatorLowerChanged;
}
bool MainStatusMessage::evaporatorUpperAirTempChanged() const
{
    return mEvaporatorUpperChanged;
}
bool MainStatusMessage::fanSpeedChanged() const
{
    return mFanSpeedChanged;
}
bool MainStatusMessage::statesChanged() const
{
    return mStatesChanged;
}
bool MainStatusMessage::settingPwmFirstChanged() const
{
    return mSettingPwmFirstChanged;
}
bool MainStatusMessage::settingPwmSecondChanged() const
{
    return mSettingPwmSecondChanged;
}
bool MainStatusMessage::settingPwmThirdChanged() const
{
    return mSettingPwmThirdChanged;
}
bool MainStatusMessage::settingMinTTargetChanged() const
{
    return mSettingMinTChanged;
}
bool MainStatusMessage::settingLegionellaTTargetChanged() const
{
    return mSettingAntiLegionellaTargetChanged;
}
bool MainStatusMessage::settingWattageHeatingElementChanged() const
{
    return mSettingWattageHeatElementChanged;
}
bool MainStatusMessage::settingBoilerCapacityChanged() const
{
    return mSettingBoilerCapacityChanged;
}
bool MainStatusMessage::settingBrandChanged() const
{
    return mSettingBoilerBrandChanged;
}
bool MainStatusMessage::settingCapabilitiesChanged() const
{
    return mSettingCapabilitiesChanged;
}
bool MainStatusMessage::statePVChanged() const
{
    return mPVStateChanged;
}
}  // namespace message
}  // namespace aquamqtt