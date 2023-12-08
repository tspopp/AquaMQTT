#ifndef AQUAMQTT_HMIMESSAGE_H
#define AQUAMQTT_HMIMESSAGE_H

#include <cstring>

#include "MessageConstants.h"

namespace aquamqtt
{
namespace message
{
class HMIMessage
{
public:
    explicit HMIMessage(uint8_t* data);

    ~HMIMessage() = default;

    float waterTempTarget();

    void setWaterTempTarget(float targetTemperature);

    HMIOperationMode operationMode();

    void setOperationMode(HMIOperationMode operationMode);

    bool isTimerModeEnabled();

    void setTimerModeEnabled(boolean enabled);

    // TODO: sanity you cannot active emergency mode if heating element is disabled
    bool isEmergencyModeEnabled();

    // TODO: you cannot disable heating elment if emergency mode is activated
    bool isHeatingElementEnabled();

    // TODO: implement
    bool setupModeA();

    // TODO: implement
    bool setupModeB();

    uint8_t antiLegionellaModePerMonth();

    HMIAirDuctConfig airDuctConfig();

    HMIInstallation installationMode();

    HMITestMode testMode();

    // TODO: sanity, timer window cannot exceed 14 hours (both a and b)
    // 1st range duration: 4 hours < time < 14 hours;
    // Total duration of the 2 ranges: 8 hours minimum and 14 hours maximum.
    // FIXME: do not return string
    std::string timerWindowStr(bool firstWindow);

    uint16_t timerWindowAStart();

    uint16_t timerWindowALength();

    uint16_t timerWindowBStart();

    uint16_t timerWindowBLength();

    uint8_t timeHours();

    void setTimeHours(uint8_t hour);

    uint8_t timeMinutes();

    void setTimeMinutes(uint8_t minute);

    uint8_t timeSeconds();

    void setTimeSeconds(uint8_t second);

    uint16_t dateYear();

    void setDateMonthAndYear(uint8_t month, uint16_t year);

    uint8_t dateMonth();

    uint8_t dateDay();

    void setDateDay(uint8_t day);

private:
    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_HMIMESSAGE_H
