#ifndef AQUAMQTT_ERRORMESSAGE_H
#define AQUAMQTT_ERRORMESSAGE_H

#include <cstring>

#include "message/IErrorMessage.h"

namespace aquamqtt
{
namespace message
{
namespace next
{
class ErrorMessage : public IErrorMessage
{
public:
    explicit ErrorMessage(uint8_t* data);

    ~ErrorMessage() override = default;

    uint8_t getLength() override;

    uint8_t errorRequestId() override;

    uint8_t errorCode() override;

    float hotWaterTemp() override;

    float airTemp() override;

    float evaporatorLowerAirTemp() override;

    float evaporatorUpperAirTemp() override;

    float fanSpeedPwm() override;

    uint16_t totalHeatpumpHours() override;

    uint16_t totalHeatingElemHours() override;

    uint8_t timeHours() override;

    uint8_t timeMinutes() override;

    uint16_t dateYear() override;

    uint8_t dateMonth() override;

    uint8_t dateDay() override;

    bool isEmpty() override;

private:
    uint8_t* mData;
};

}  // namespace next
}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_ERRORMESSAGE_H
