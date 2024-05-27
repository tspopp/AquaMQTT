#ifndef AQUAMQTT_ERRORMESSAGE_H
#define AQUAMQTT_ERRORMESSAGE_H

#include <cstring>

#include "MessageConstants.h"

namespace aquamqtt
{
namespace message
{
class ErrorMessage
{
public:
    explicit ErrorMessage(uint8_t* data);

    ~ErrorMessage() = default;

    uint8_t errorRequestId();

    uint8_t errorCode();

    float hotWaterTemp();

    float airTemp();

    float evaporatorLowerAirTemp();

    float evaporatorUpperAirTemp();

    float fanSpeedPwm();

    uint16_t totalHeatpumpHours();

    uint16_t totalHeatingElemHours();

    uint8_t timeHours();

    uint8_t timeMinutes();

    uint16_t dateYear();

    uint8_t dateMonth();

    uint8_t dateDay();

    bool isEmpty();

private:
    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_ERRORMESSAGE_H
