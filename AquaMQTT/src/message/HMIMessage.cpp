#include "message/HMIMessage.h"

#include <cstdio>

namespace aquamqtt
{
namespace message
{

HMIMessage::HMIMessage(uint8_t* data)
    : mData(data)
    , mTargetTempChanged(false)
    , mOperationModeChanged(false)
    , mLegionellaAirductChanged(false)
    , mEmergencyModeChanged(false)
    , mInstallConfigChanged(false)
    , mHeatingElemOrSetupStateOrPVActiveChanged(false)
    , mTimerModeOneChanged(false)
    , mTimerModeTwoChanged(false)
    , mTimeChanged(false)
    , mDateChanged(false)
    , mTestModeChanged(false)
    , mErrorRequestChanged(false)
    , mExhaustFanChanged(false)
{
}
float HMIMessage::waterTempTarget()
{
    // TODO
    return 0.0;
}
void HMIMessage::setWaterTempTarget(float targetTemperature)
{
    // TODO
}
HMIOperationMode HMIMessage::operationMode() const
{
    // TODO
    return OM_UNKNOWN;
}
void HMIMessage::setOperationMode(HMIOperationMode operationMode) const
{
    // TODO
}
HMIOperationType HMIMessage::getOperationType() const
{
    // TODO
    return HMIOperationType::ALWAYS_ON;
}

void HMIMessage::setOperationType(HMIOperationType operationType) const
{
    // TODO
}
bool HMIMessage::isEmergencyModeEnabled() const
{
    // TODO
    return false;
}
bool HMIMessage::isHeatingElementEnabled() const
{
    // TODO
    return false;
}

bool HMIMessage::isPVInputActivated() const
{
    // TODO
    return false;
}

HMISetup HMIMessage::setupMode() const
{
    // TODO
    return HMISetup::COMPLETED;
}

uint8_t HMIMessage::antiLegionellaModePerMonth() const
{
    // TODO
    return 0;
}
HMIAirDuctConfig HMIMessage::airDuctConfig() const
{
    // TODO
    return AD_UNKNOWN;
}

void HMIMessage::setAirDuctConfig(HMIAirDuctConfig config) const
{
    // TODO
}
HMIInstallation HMIMessage::installationMode()
{
    // TODO
    return HMIInstallation::INST_HP_UNKNOWN;
}

HMIFanExhaust HMIMessage::fanExhaust() const
{
    // TODO
    return EXHAUST_UNKNOWN;
}
void HMIMessage::setFanExhaustMode(HMIFanExhaust mode) const
{
    // TODO
}
bool HMIMessage::exhaustFanChanged() const
{
    return mExhaustFanChanged;
}

void HMIMessage::setInstallationMode(HMIInstallation mode) const
{
    // TODO
}

HMITestMode HMIMessage::testMode()
{
    // TODO
    return TM_UNKNOWN;
}
void HMIMessage::timerWindowStr(bool firstWindow, char* buffer)
{
    // TODO
}
uint16_t HMIMessage::timerWindowAStart() const
{
    // TODO
    return 0;
}
uint16_t HMIMessage::timerWindowALength() const
{
    // TODO
    return 0;
}
uint16_t HMIMessage::timerWindowBStart() const
{
    // TODO
    return 0;
}
uint16_t HMIMessage::timerWindowBLength() const
{
    // TODO
    return 0;
}
uint8_t HMIMessage::timeHours() const
{
    return mData[20];
}
void HMIMessage::setTimeHours(uint8_t hour) const
{
    // TODO
}
uint8_t HMIMessage::timeMinutes() const
{
    return mData[19];
}
void HMIMessage::setTimeMinutes(uint8_t minute) const
{
    // TODO
}
uint8_t HMIMessage::timeSeconds() const
{
    return mData[16];
}
void HMIMessage::setTimeSeconds(uint8_t second) const
{
    // TODO
}
uint16_t HMIMessage::dateYear() const
{
    return 2000 + (mData[18] / 2);
}
void HMIMessage::setDateMonthAndYear(uint8_t month, uint16_t year) const
{
    // TODO
}
uint8_t HMIMessage::dateMonth() const
{
    return (mData[17] >> 5) + ((mData[18] % 2) * 8);
}
uint8_t HMIMessage::dateDay() const
{
    return mData[17] & 0x1F;
}
void HMIMessage::setDateDay(uint8_t day) const
{
    // TODO
}

void HMIMessage::setEmergencyMode(bool enabled) const
{
    // TODO
}

void HMIMessage::enableHeatingElement(bool enabled) const
{
    // TODO
}

// TODO: implement me
// TODO: sanity, timer window cannot exceed 14 hours (both a and b)
// 1st range duration: 4 hours < time < 14 hours;
// Total duration of the 2 ranges: 8 hours minimum and 14 hours maximum.
void HMIMessage::setTimeWindowByStr(bool firstWindow, char* buffer, uint8_t length)
{
}

// TODO: implement me
void HMIMessage::setAntiLegionellaModePerMonth(uint8_t value)
{
}

uint8_t HMIMessage::errorRequestId() const
{
    // TODO
    return 0;
}
uint8_t HMIMessage::errorNumberRequested() const
{
    // TODO
    return 0;
}

void HMIMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        mTargetTempChanged                        = true;
        mOperationModeChanged                     = true;
        mLegionellaAirductChanged                 = true;
        mEmergencyModeChanged                     = true;
        mInstallConfigChanged                     = true;
        mHeatingElemOrSetupStateOrPVActiveChanged = true;
        mTimerModeOneChanged                      = true;
        mTimerModeTwoChanged                      = true;
        mTimeChanged                              = true;
        mDateChanged                              = true;
        mTestModeChanged                          = true;
        mErrorRequestChanged                      = true;
        mExhaustFanChanged                        = true;
        return;
    }

    uint8_t diffIndices[HMI_MESSAGE_LENGTH] = { 0 };
    size_t  numDiffs                        = 0;
    compareBuffers(mData, data, HMI_MESSAGE_LENGTH, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 16:
            case 19:
            case 20:
                mTimeChanged = true;
                break;
            case 17:
            case 18:
                mDateChanged = true;
                break;
            default:
                break;
        }
    }
}
bool HMIMessage::waterTempTargetChanged() const
{
    return mTargetTempChanged;
}
bool HMIMessage::operationTypeOrModeChanged() const
{
    return mOperationModeChanged;
}
bool HMIMessage::timeChanged() const
{
    return mTimeChanged;
}
bool HMIMessage::dateChanged() const
{
    return mDateChanged;
}
bool HMIMessage::emergencyModeChanged() const
{
    return mEmergencyModeChanged;
}
bool HMIMessage::heatingElemOrSetupStateOrPVActiveChanged() const
{
    return mHeatingElemOrSetupStateOrPVActiveChanged;
}
bool HMIMessage::legionellaOrAirductChanged() const
{
    return mLegionellaAirductChanged;
}
bool HMIMessage::testModeChanged() const
{
    return mTestModeChanged;
}
bool HMIMessage::installationConfigChanged() const
{
    return mInstallConfigChanged;
}
bool HMIMessage::timerModeOneChanged() const
{
    return mTimerModeOneChanged;
}
bool HMIMessage::timerModeTwoChanged() const
{
    return mTimerModeTwoChanged;
}

bool HMIMessage::errorRequestChanged() const
{
    return mErrorRequestChanged;
}

}  // namespace message
}  // namespace aquamqtt