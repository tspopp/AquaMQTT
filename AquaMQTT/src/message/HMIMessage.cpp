#include "message/HMIMessage.h"

#include <cstdio>

namespace aquamqtt
{
namespace message
{

HMIMessage::HMIMessage(uint8_t* data) : mData(data)
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
HMIOperationMode HMIMessage::operationMode()
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
void HMIMessage::setOperationMode(HMIOperationMode operationMode)
{
    if (operationMode != HMIOperationMode::OM_UNKNOWN)
    {
        mData[3] = (mData[3] & 0xF0) | (operationMode & 0x0F);
    }
}
HMIOperationType HMIMessage::getOperationType()
{
    if (mData[3] & 0x40)
    {
        return HMIOperationType::TIMER;
    }
    return HMIOperationType::ALWAYS_ON;
}

void HMIMessage::setOperationType(HMIOperationType operationType)
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
bool HMIMessage::isEmergencyModeEnabled()
{
    return mData[6] & 0x01;
}
bool HMIMessage::isHeatingElementEnabled()
{
    return mData[9] & 0x04;
}
HMISetup HMIMessage::setupMode()
{
    if (mData[9] & 0x80)
    {
        return HMISetup::RESET;
    }
    else if (mData[9] & 0x20)
    {
        return HMISetup::INCOMPLETE;
    }
    else
    {
        return HMISetup::COMPLETED;
    }
}

uint8_t HMIMessage::antiLegionellaModePerMonth()
{
    return (uint8_t) (mData[5] & 0x0F);
}
HMIAirDuctConfig HMIMessage::airDuctConfig()
{
    switch ((uint8_t) (mData[5] & 0xF0))
    {
        case 0:
            return HMIAirDuctConfig::AD_INT_INT;
        case 16:
            return HMIAirDuctConfig::AD_INT_EXT;
        case 32:
            return HMIAirDuctConfig::AD_EXT_EXT;
        default:
            return HMIAirDuctConfig::AD_UNKNOWN;
    }
}
HMIInstallation HMIMessage::installationMode()
{
    if (mData[7] & 0x02)
    {
        return HMIInstallation::INST_HP_AND_SOLAR;
    }
    else if (mData[7] & 0x01)
    {
        if (mData[7] & 0x10 && mData[7] & 0x20)
        {
            return HMIInstallation::INST_HP_AND_EXT_PRIO_EXT;
        }
        else if (!(mData[7] & 0x10) && mData[7] & 0x20)
        {
            return HMIInstallation::INST_HP_AND_EXT_OPT_EXT;
        }
        else if (mData[7] & 0x10 && !(mData[7] & 0x20))
        {
            return HMIInstallation::INST_HP_AND_EXT_OPT_HP;
        }
        else
        {
            return HMIInstallation::INST_HP_AND_EXT_PRIO_HP;
        }
    }
    else
    {
        return HMIInstallation::INST_HP_ONLY;
    }
}

void HMIMessage::setInstallationMode(HMIInstallation mode)
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
            return HMITestMode::TM_OFF;
        case 1:
            return HMITestMode::TM_ENTERED;
        case 2:
            return HMITestMode::TM_HEAT_PUMP;
        case 3:
            return HMITestMode::TM_HEAT_ELEMENT;
        case 4:
            return HMITestMode::TM_FAN_LOW;
        case 5:
            return HMITestMode::TM_FAN_HIGH;
        case 6:
            return HMITestMode::TM_DEFROST;
        case 8:
            return HMITestMode::TM_HEAT_PUMP_AND_EXT;
        default:
            return HMITestMode::TM_UNKNOWN;
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
uint16_t HMIMessage::timerWindowAStart()
{
    return mData[10];
}
uint16_t HMIMessage::timerWindowALength()
{
    return mData[11];
}
uint16_t HMIMessage::timerWindowBStart()
{
    return mData[12];
}
uint16_t HMIMessage::timerWindowBLength()
{
    return mData[13];
}
uint8_t HMIMessage::timeHours()
{
    return mData[21];
}
void HMIMessage::setTimeHours(uint8_t hour)
{
    mData[21] = hour;
}
uint8_t HMIMessage::timeMinutes()
{
    return mData[20];
}
void HMIMessage::setTimeMinutes(uint8_t minute)
{
    mData[20] = minute;
}
uint8_t HMIMessage::timeSeconds()
{
    return mData[17];
}
void HMIMessage::setTimeSeconds(uint8_t second)
{
    mData[17] = second;
}
uint16_t HMIMessage::dateYear()
{
    return 2000 + (mData[19] / 2);
}
void HMIMessage::setDateMonthAndYear(uint8_t month, uint16_t year)
{
    int pastJuly   = month > 7 ? 1 : 0;
    mData[19]      = ((year - 2000) * 2) + pastJuly;
    int monthValue = (pastJuly ? month - 8 : month) << 5;
    mData[18]      = (mData[18] & 0x1F) | (monthValue & 0xE0);
}
uint8_t HMIMessage::dateMonth()
{
    return (mData[18] >> 5) + ((mData[19] % 2) * 8);
}
uint8_t HMIMessage::dateDay()
{
    return mData[18] & 0x1F;
}
void HMIMessage::setDateDay(uint8_t day)
{
    mData[18] = (mData[18] & 0xE0) | (day & 0x1F);
}

void HMIMessage::setEmergencyMode(bool enabled)
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

void HMIMessage::enableHeatingElement(bool enabled)
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

}  // namespace message
}  // namespace aquamqtt