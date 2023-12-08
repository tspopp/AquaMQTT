#include "message/HMIMessage.h"

namespace aquamqtt
{
namespace message
{

HMIMessage::HMIMessage(uint8_t* data) : mData(data)
{
}
float HMIMessage::waterTempTarget()
{
    return (float) (((int16_t) mData[2] << 8) | mData[1]) / 10.0;
}
void HMIMessage::setWaterTempTarget(float targetTemperature)
{
    mData[1] = (int) targetTemperature * 10 & 0xFF;
    mData[2] = ((int) targetTemperature * 10 >> 8) & 0xFF;
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
bool HMIMessage::isTimerModeEnabled()
{
    return mData[3] & 0x40;
}
void HMIMessage::setTimerModeEnabled(boolean enabled)
{
    mData[3] = (mData[3] & ~(1 << 6)) | (enabled << 6);
}
bool HMIMessage::isEmergencyModeEnabled()
{
    return mData[6] & 0x01;
}
bool HMIMessage::isHeatingElementEnabled()
{
    return mData[9] & 0x04;
}
bool HMIMessage::setupModeA()
{
    // Fresh-Reset = 164 / 1010 0100? (nothing set)
    return mData[9] & 0x80;
}
bool HMIMessage::setupModeB()
{
    // Then 36             0010 0100 (umluftbetrieb set, rest missing
    return mData[9] & 0x20;
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
    switch (mData[7])
    {
        case 0:
            return HMIInstallation::INST_HP_ONLY;
        case 1:
            return HMIInstallation::INST_HP_AND_EXT_PRIO_HP;
        case 17:
            return HMIInstallation::INST_HP_AND_EXT_OPT_HP;
        case 33:
            return HMIInstallation::INST_HP_AND_EXT_OPT_EXT;
        case 49:
            return HMIInstallation::INST_HP_AND_EXT_PRIO_EXT;
        case 50:
            return HMIInstallation::INST_HP_AND_SOLAR;
        default:
            return HMIInstallation::INST_HP_UNKNOWN;
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
std::string HMIMessage::timerWindowStr(bool firstWindow)
{
    uint16_t start    = firstWindow ? timerWindowAStart() : timerWindowBStart();
    uint16_t duration = firstWindow ? timerWindowALength() : timerWindowBLength();

    char          beginStr[13];
    const uint8_t beginHours    = (start * 15) / 60;
    const uint8_t beginMinutes  = (start * 15) % 60;
    const uint8_t lengthHours   = (duration * 15) / 60;
    const uint8_t lengthMinutes = (duration * 15) % 60;
    const uint8_t endMinutes    = (beginMinutes + lengthMinutes) % 60;
    const uint8_t hourOverlap   = (beginMinutes + lengthMinutes) / 60;
    const uint8_t endHours      = (beginHours + lengthHours) % 24 + hourOverlap;
    sprintf(beginStr, "%02d:%02d-%02d:%02d", beginHours, beginMinutes, endHours, endMinutes);
    return beginStr;
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
}  // namespace message
}  // namespace aquamqtt