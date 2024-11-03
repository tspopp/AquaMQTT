#include "message/MainStatusMessage.h"

namespace aquamqtt
{
namespace message
{

float message::MainStatusMessage::hotWaterTemp()
{
    // TODO
    return 0;
}

void MainStatusMessage::setHotWaterTemp(float temp)
{
    // TODO
    return;
}

float MainStatusMessage::airTemp()
{
    // TODO
    return 0;
}

void MainStatusMessage::setAirTemp(float temp)
{
    // TODO
    return;
}

float MainStatusMessage::evaporatorLowerAirTemp()
{
    // TODO
    return 0;
}

void MainStatusMessage::setEvaporatorLowerAirTemp(float temp)
{
    // TODO
    return;
}

float MainStatusMessage::evaporatorUpperAirTemp()
{
    // TODO
    return 0;
}

void MainStatusMessage::setEvaporatorUpperAirTemp(float temp)
{  // TODO
    return;
}

float MainStatusMessage::fanSpeedPwm()
{
    // TODO
    return 0;
}

bool MainStatusMessage::stateHeatingElement()
{
    // TODO
    return false;
}
bool MainStatusMessage::stateHeatpump()
{
    // TODO
    return false;
}
bool MainStatusMessage::stateBoilerBackup()
{
    // TODO
    return false;
}
bool MainStatusMessage::stateFan()
{
    // TODO
    return false;
}
bool MainStatusMessage::stateDefrost()
{
    // TODO
    return false;
}

bool MainStatusMessage::statePV()
{
    // TODO
    return false;
}

bool MainStatusMessage::stateSolar()
{
    // TODO
    return false;
}

uint8_t MainStatusMessage::settingMinTTarget()
{
    // TODO
    return 0;
}
uint8_t MainStatusMessage::settingPwmFirst()
{
    // TODO
    return 0;
}
uint8_t MainStatusMessage::settingPwmSecond()
{
    // TODO
    return 0;
}
uint8_t MainStatusMessage::settingPwmThird()
{
    // TODO
    return 0;
}
uint8_t MainStatusMessage::settingLegionellaTTarget()
{
    // TODO
    return 0;
}
uint16_t MainStatusMessage::settingWattageHeatingElement()
{
    // TODO
    return 0;
}
uint16_t MainStatusMessage::settingBoilerCapacity()
{
    // TODO
    return 0;
}
MAINBrands MainStatusMessage::settingBrand()
{
    // TODO
    return MAINBrands::BR_UNKNOWN;
}

bool MainStatusMessage::capabilityHasHeatExchanger()
{
    // TODO
    return false;
}
bool MainStatusMessage::capabilityHasCirculation()
{
    // TODO
    return false;
}
bool MainStatusMessage::capabilityHasPVInput()
{
    // TODO
    return false;
}
bool MainStatusMessage::capabilityHasCommunication()
{
    // TODO
    return false;
}
bool MainStatusMessage::capabilityHasAntiDryHeating()
{
    // TODO
    return false;
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
        mPVOrSolarStateChanged              = true;
        mErrorCodeChanged                   = true;
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
    , mPVOrSolarStateChanged(false)
    , mErrorCodeChanged(false)
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
bool MainStatusMessage::statePVOrSolarChanged() const
{
    return mPVOrSolarStateChanged;
}
bool MainStatusMessage::errorCodeChanged() const
{
    return mErrorCodeChanged;
}
uint8_t MainStatusMessage::errorCode() const
{
    if (mData[23] == UINT8_MAX)
    {
        return 0;
    }
    return mData[23];
}
void MainStatusMessage::enableStatePV(bool enabled)
{
    if (enabled)
    {
        mData[22] |= 0x10;
    }
    else
    {
        mData[22] &= ~0x10;
    }
}
void MainStatusMessage::enableStateSolar(bool enabled)
{
    if (enabled)
    {
        mData[22] |= 0x20;
    }
    else
    {
        mData[22] &= ~0x20;
    }
}
}  // namespace message
}  // namespace aquamqtt