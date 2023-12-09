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

    bool isEmergencyModeEnabled();

    void setEmergencyMode(bool enabled);

    bool isHeatingElementEnabled();

    void enableHeatingElement(bool enabled);

    HMISetup setupMode();

    uint8_t antiLegionellaModePerMonth();

    void setAntiLegionellaModePerMonth(uint8_t value);

    HMIAirDuctConfig airDuctConfig();

    HMIInstallation installationMode();

    HMITestMode testMode();

    void timerWindowStr(bool firstWindow, char* buffer);

    void setTimeWindowByStr(bool firstWindow, char* buffer, uint8_t length);

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
