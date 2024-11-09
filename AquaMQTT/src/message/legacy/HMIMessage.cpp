#include "message/legacy/HMIMessage.h"

#include <cstdio>

namespace aquamqtt
{
namespace message
{
namespace legacy
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
    return (float) (((short int) (mData[2] << 8) | mData[1]) / 10.0);
}
void HMIMessage::setWaterTempTarget(float targetTemperature)
{
    short int rawValue = targetTemperature * 100 / 10;
    mData[1]           = rawValue & 0xFF;
    mData[2]           = (rawValue >> 8) & 0xFF;
}
HMIOperationMode HMIMessage::operationMode() const
{
    switch (mData[3] & 0x0F)
    {
        case 0:
            return OM_ABSENCE;
        case 1:
            return OM_ECO_ACTIVE;
        case 2:
            return OM_ECO_INACTIVE;
        case 3:
            return OM_BOOST;
        case 4:
            return OM_AUTO;
        default:
            return OM_UNKNOWN;
    }
}
void HMIMessage::setOperationMode(HMIOperationMode operationMode) const
{
    if (operationMode != HMIOperationMode::OM_UNKNOWN)
    {
        mData[3] = (mData[3] & 0xF0) | (operationMode & 0x0F);
    }
}
HMIOperationType HMIMessage::getOperationType() const
{
    if (mData[3] & 0x40)
    {
        return HMIOperationType::TIMER;
    }
    return HMIOperationType::ALWAYS_ON;
}

void HMIMessage::setOperationType(HMIOperationType operationType) const
{
    if (operationType == HMIOperationType::TIMER)
    {
        mData[3] = (mData[3] & ~(1 << 6)) | (true << 6);
    }
    else
    {
        mData[3] = (mData[3] & ~(1 << 6)) | (false << 6);
    }
}
bool HMIMessage::isEmergencyModeEnabled() const
{
    return mData[6] & 0x01;
}
bool HMIMessage::isHeatingElementEnabled() const
{
    return mData[9] & 0x04;
}

bool HMIMessage::isPVInputActivated() const
{
    return mData[9] & 0x02;
}

HMISetup HMIMessage::setupMode() const
{
    if (mData[9] & 0x80)
    {
        return HMISetup::RESET;
    }
    if (mData[9] & 0x20)
    {
        return HMISetup::INCOMPLETE;
    }
    return HMISetup::COMPLETED;
}

uint8_t HMIMessage::antiLegionellaModePerMonth() const
{
    return (uint8_t) (mData[5] & 0x0F);
}
HMIAirDuctConfig HMIMessage::airDuctConfig() const
{
    switch ((uint8_t) (mData[5] & 0xF0))
    {
        case 0:
            return AD_INT_INT;
        case 16:
            return AD_INT_EXT;
        case 32:
            return AD_EXT_EXT;
        default:
            return AD_UNKNOWN;
    }
}

void HMIMessage::setAirDuctConfig(HMIAirDuctConfig config) const
{
    if (config == AD_UNKNOWN)
    {
        return;
    }

    // clear bit 5 and 6 (INT/INT)
    mData[5] &= 0b11001111;

    if (config == AD_INT_EXT)
    {
        mData[5] |= 0b00010000;
    }
    else if (config == AD_EXT_EXT)
    {
        mData[5] |= 0b00100000;
    }
}
HMIInstallation HMIMessage::installationMode()
{
    if (mData[7] & 0x02)
    {
        return INST_HP_AND_SOLAR;
    }
    if (mData[7] & 0x01)
    {
        if (mData[7] & 0x10 && mData[7] & 0x20)
        {
            return HMIInstallation::INST_HP_AND_EXT_PRIO_EXT;
        }
        if (!(mData[7] & 0x10) && mData[7] & 0x20)
        {
            return HMIInstallation::INST_HP_AND_EXT_OPT_EXT;
        }
        if (mData[7] & 0x10 && !(mData[7] & 0x20))
        {
            return HMIInstallation::INST_HP_AND_EXT_OPT_HP;
        }
        return HMIInstallation::INST_HP_AND_EXT_PRIO_HP;
    }
    return HMIInstallation::INST_HP_ONLY;
}

HMIFanExhaust HMIMessage::fanExhaust() const
{
    switch ((uint8_t) (mData[8] & 0x03))
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
    if (mode == EXHAUST_UNKNOWN)
        return;

    // clean the first two bits
    mData[8] &= 0b11111100;
    // apply value
    mData[8] |= mode;
}
bool HMIMessage::exhaustFanChanged() const
{
    return mExhaustFanChanged;
}

void HMIMessage::setInstallationMode(HMIInstallation mode) const
{
    switch (mode)
    {
        case INST_HP_ONLY:
            mData[7] &= ~0x01;
            mData[7] &= ~0x02;
            mData[7] &= ~0x10;
            mData[7] &= ~0x20;
            break;
        case INST_HP_AND_EXT_PRIO_HP:
            mData[7] |= 0x01;
            mData[7] &= ~0x02;
            mData[7] &= ~0x10;
            mData[7] &= ~0x20;
            break;
        case INST_HP_AND_EXT_OPT_HP:
            mData[7] |= 0x01;
            mData[7] &= ~0x02;
            mData[7] |= 0x10;
            mData[7] &= ~0x20;
            break;
        case INST_HP_AND_EXT_OPT_EXT:
            mData[7] |= 0x01;
            mData[7] &= ~0x02;
            mData[7] &= ~0x10;
            mData[7] |= 0x20;
            break;
        case INST_HP_AND_EXT_PRIO_EXT:
            mData[7] |= 0x01;
            mData[7] &= ~0x02;
            mData[7] |= 0x10;
            mData[7] |= 0x20;
            break;
        case INST_HP_AND_SOLAR:
            mData[7] |= 0x02;
            mData[7] &= ~0x01;
            mData[7] &= ~0x10;
            mData[7] &= ~0x20;
            break;
        default:
            break;
    }
}

HMITestMode HMIMessage::testMode()
{
    switch (mData[22])
    {
        case 0:
            return TM_OFF;
        case 1:
            return TM_ENTERED;
        case 2:
            return TM_HEAT_PUMP;
        case 3:
            return TM_HEAT_ELEMENT;
        case 4:
            return TM_FAN_LOW;
        case 5:
            return TM_FAN_HIGH;
        case 6:
            return TM_DEFROST;
        case 8:
            return TM_HEAT_PUMP_AND_EXT;
        default:
            return TM_UNKNOWN;
    }
}
void HMIMessage::timerWindowStr(bool firstWindow, char* buffer)
{
    uint16_t start    = firstWindow ? timerWindowAStart() : timerWindowBStart();
    uint16_t duration = firstWindow ? timerWindowALength() : timerWindowBLength();

    const uint8_t beginHours    = (start * 15) / 60;
    const uint8_t beginMinutes  = (start * 15) % 60;
    const uint8_t lengthHours   = (duration * 15) / 60;
    const uint8_t lengthMinutes = (duration * 15) % 60;
    const uint8_t endMinutes    = (beginMinutes + lengthMinutes) % 60;
    const uint8_t hourOverlap   = (beginMinutes + lengthMinutes) / 60;
    const uint8_t endHours      = (beginHours + lengthHours) % 24 + hourOverlap;
    sprintf(buffer, "%02d:%02d-%02d:%02d", beginHours, beginMinutes, endHours, endMinutes);
}
uint16_t HMIMessage::timerWindowAStart() const
{
    return mData[10];
}
uint16_t HMIMessage::timerWindowALength() const
{
    return mData[11];
}
uint16_t HMIMessage::timerWindowBStart() const
{
    return mData[12];
}
uint16_t HMIMessage::timerWindowBLength() const
{
    return mData[13];
}
uint8_t HMIMessage::timeHours() const
{
    return mData[21];
}
void HMIMessage::setTimeHours(uint8_t hour) const
{
    mData[21] = hour;
}
uint8_t HMIMessage::timeMinutes() const
{
    return mData[20];
}
void HMIMessage::setTimeMinutes(uint8_t minute) const
{
    mData[20] = minute;
}
uint8_t HMIMessage::timeSeconds() const
{
    return mData[17];
}
void HMIMessage::setTimeSeconds(uint8_t second) const
{
    mData[17] = second;
}
uint16_t HMIMessage::dateYear() const
{
    return 2000 + (mData[19] / 2);
}
void HMIMessage::setDateMonthAndYear(uint8_t month, uint16_t year) const
{
    int pastJuly   = month > 7 ? 1 : 0;
    mData[19]      = ((year - 2000) * 2) + pastJuly;
    int monthValue = (pastJuly ? month - 8 : month) << 5;
    mData[18]      = (mData[18] & 0x1F) | (monthValue & 0xE0);
}
uint8_t HMIMessage::dateMonth() const
{
    return (mData[18] >> 5) + ((mData[19] % 2) * 8);
}
uint8_t HMIMessage::dateDay() const
{
    return mData[18] & 0x1F;
}
void HMIMessage::setDateDay(uint8_t day) const
{
    mData[18] = (mData[18] & 0xE0) | (day & 0x1F);
}

void HMIMessage::setEmergencyMode(bool enabled) const
{
    // Sanity: You cannot activate emergency mode if heating element is disabled
    if (enabled && isHeatingElementEnabled())
    {
        mData[6] |= 0x01;
    }
    else if (!enabled)
    {
        mData[6] &= ~0x01;
    }
}

void HMIMessage::enableHeatingElement(bool enabled) const
{
    if (enabled)
    {
        mData[9] |= 0x04;
    }
    // Sanity: You cannot disable heating element if emergency mode is activated
    else if (!isEmergencyModeEnabled())
    {
        mData[9] &= ~0x04;
    }
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
    return mData[29];
}
uint8_t HMIMessage::errorNumberRequested() const
{
    return mData[28];
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

    uint8_t diffIndices[HMI_MESSAGE_LENGTH_LEGACY] = { 0 };
    size_t  numDiffs                        = 0;
    compareBuffers(mData, data, HMI_MESSAGE_LENGTH_LEGACY, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 1:
            case 2:
                mTargetTempChanged = true;
                break;
            case 3:
                mOperationModeChanged = true;
                break;
            case 5:
                mLegionellaAirductChanged = true;
                break;
            case 6:
                mEmergencyModeChanged = true;
                break;
            case 8:
                mExhaustFanChanged = true;
                break;
            case 7:
                mInstallConfigChanged = true;
                break;
            case 9:
                mHeatingElemOrSetupStateOrPVActiveChanged = true;
                break;
            case 10:
            case 11:
                mTimerModeOneChanged = true;
                break;
            case 12:
            case 13:
                mTimerModeTwoChanged = true;
                break;
            case 17:
            case 20:
            case 21:
                mTimeChanged = true;
                break;
            case 18:
            case 19:
                mDateChanged = true;
                break;
            case 22:
                mTestModeChanged = true;
                break;
            case 27:
            case 28:
                mErrorRequestChanged = true;
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
uint8_t HMIMessage::getLength()
{
    return HMI_MESSAGE_LENGTH_LEGACY;
}
}  // namespace legacy
}  // namespace message
}  // namespace aquamqtt