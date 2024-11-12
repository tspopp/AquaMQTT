#include "message/next/HMIMessage.h"

#include <cstdio>

namespace aquamqtt
{
namespace message
{
namespace next
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
    return mData[1];
}
void HMIMessage::setWaterTempTarget(float targetTemperature)
{
    // TODO
}
HMIOperationMode HMIMessage::operationMode() const
{
    switch (mData[2] & 0x0F)
    {
        case 0:
            return OM_AUTO;
        case 1:
            return OM_ECO_ACTIVE;
        case 2:
            return OM_ECO_INACTIVE;
        case 9:
            return OM_BOOST;
        case 5:
            return OM_ABSENCE;
        default:
            return OM_UNKNOWN;
    }
}
void HMIMessage::setOperationMode(HMIOperationMode operationMode) const
{
    // TODO
    // TODO: Sanity: If Mode OM_AUTO is set, water target temperature is set to 0x00
}
HMIOperationType HMIMessage::getOperationType() const
{
    if (mData[3] & 0x40)
    {
        return HMIOperationType::ALWAYS_ON;
    }
    return HMIOperationType::TIMER;
}

void HMIMessage::setOperationType(HMIOperationType operationType) const
{
    // TODO
}
bool HMIMessage::isEmergencyModeEnabled() const
{
    return mData[5] & 0x01;
}
bool HMIMessage::isHeatingElementEnabled() const
{
    return mData[8] & 0x04;
}

bool HMIMessage::isPVInputActivated() const
{
    return mData[8] & 0x02;
}

HMISetup HMIMessage::setupMode() const
{
    // TODO
    return HMISetup::COMPLETED;
}

uint8_t HMIMessage::antiLegionellaModePerMonth() const
{
    return mData[4] & 0x0F;
}
HMIAirDuctConfig HMIMessage::airDuctConfig() const
{
    switch (mData[4] & 0xF0)
    {
        case 0:
            return AD_INT_INT;
        case 1:
            return AD_INT_EXT;
        case 2:
            return AD_EXT_EXT;
        default:
            return AD_UNKNOWN;
    }
}

void HMIMessage::setAirDuctConfig(HMIAirDuctConfig config) const
{
    // TODO
}
HMIInstallation HMIMessage::installationMode()
{
    if (mData[6] & 0x02)
    {
        return INST_HP_AND_SOLAR;
    }
    if (mData[6] & 0x01)
    {
        if (mData[6] & 0x10 && mData[6] & 0x20)
        {
            return HMIInstallation::INST_HP_AND_EXT_PRIO_EXT;
        }
        if (!(mData[6] & 0x10) && mData[6] & 0x20)
        {
            return HMIInstallation::INST_HP_AND_EXT_OPT_EXT;
        }
        if (mData[6] & 0x10 && !(mData[6] & 0x20))
        {
            return HMIInstallation::INST_HP_AND_EXT_OPT_HP;
        }
        return HMIInstallation::INST_HP_AND_EXT_PRIO_HP;
    }
    return HMIInstallation::INST_HP_ONLY;
}

HMIFanExhaust HMIMessage::fanExhaust() const
{
    switch ((uint8_t) (mData[7] & 0x03))
    {
        case 0:
            return EXHAUST_STOP;
        case 1:
            return EXHAUST_LOW_SPEED;
        case 2:
            return EXHAUST_HIGH_SPEED;
        default:
            return EXHAUST_UNKNOWN;
    }
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
    uint16_t start    = firstWindow ? timerWindowAStart() : timerWindowBStart();
    uint16_t duration = firstWindow ? timerWindowALength() : timerWindowBLength();

    const uint8_t beginHours    = start / 60;
    const uint8_t beginMinutes  = start % 60;
    const uint8_t lengthHours   = duration / 60;
    const uint8_t lengthMinutes = duration % 60;
    const uint8_t endMinutes    = (beginMinutes + lengthMinutes) % 60;
    const uint8_t hourOverlap   = (beginMinutes + lengthMinutes) / 60;
    const uint8_t endHours      = (beginHours + lengthHours) % 24 + hourOverlap;
    sprintf(buffer, "%02d:%02d-%02d:%02d", beginHours, beginMinutes, endHours, endMinutes);
}
uint16_t HMIMessage::timerWindowAStart() const
{
   return  ((uint16_t) mData[10] << 8) | (uint16_t) mData[9];
}
uint16_t HMIMessage::timerWindowALength() const
{
    return ((uint16_t) mData[12] << 8) | (uint16_t) mData[11];
}
uint16_t HMIMessage::timerWindowBStart() const
{
    return  ((uint16_t) mData[30] << 8) | (uint16_t) mData[29];
}
uint16_t HMIMessage::timerWindowBLength() const
{
    return ((uint16_t) mData[32] << 8) | (uint16_t) mData[31];
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
    return 1 + (mData[17] >> 5) + ((mData[18] % 2) * 8);
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
    return mData[28];
}

uint8_t HMIMessage::errorNumberRequested() const
{
    return mData[27];
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

    uint8_t diffIndices[HMI_MESSAGE_LENGTH_NEXT] = { 0 };
    size_t  numDiffs                             = 0;
    compareBuffers(mData, data, HMI_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 1:
                mTargetTempChanged = true;
                break;
            case 2:
                mOperationModeChanged = true;
                break;
            case 4:
                mLegionellaAirductChanged = true;
                break;
            case 5:
                mEmergencyModeChanged = true;
                break;
            case 6:
                mInstallConfigChanged = true;
                break;
            case 7:
                mExhaustFanChanged = true;
                break;
            case 8:
                mHeatingElemOrSetupStateOrPVActiveChanged = true;
                break;
            case 9:
            case 10:
            case 11:
            case 12:
                mTimerModeOneChanged = true;
                break;
            case 29:
            case 30:
            case 31:
            case 32:
                mTimerModeTwoChanged = true;
                break;

            case 16:
            case 19:
            case 20:
                mTimeChanged = true;
                break;
            case 17:
            case 18:
                mDateChanged = true;
                break;
            case 27:
            case 28:
                mErrorRequestChanged = true;
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
uint8_t HMIMessage::getLength()
{
    return HMI_MESSAGE_LENGTH_NEXT;
}
}  // namespace next
}  // namespace message
}  // namespace aquamqtt