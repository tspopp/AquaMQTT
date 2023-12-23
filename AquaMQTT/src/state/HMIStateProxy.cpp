#include "state/HMIStateProxy.h"

#include "config/Configuration.h"

namespace aquamqtt
{

HMIStateProxy& HMIStateProxy::getInstance()
{
    static HMIStateProxy instance;
    return instance;
}

HMIStateProxy::HMIStateProxy()
    : IMQTTCallback()
    , mMutex(xSemaphoreCreateMutex())
    , mNotify(nullptr)
    , mOperationMode(nullptr)
    , mTargetTemperature(nullptr)
    , mTimeIsSet(false)
{
}

void HMIStateProxy::setListener(TaskHandle_t handle)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mNotify = handle;

    xSemaphoreGive(mMutex);
}

void HMIStateProxy::applyHMIOverrides(uint8_t* buffer)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    message::HMIMessage message(buffer);

    if (mTargetTemperature != nullptr && *mTargetTemperature <= 62.0f && *mTargetTemperature >= 20.0f)
    {
        message.setWaterTempTarget(*mTargetTemperature);
    }

    if (mOperationMode != nullptr)
    {
        message.setOperationMode(*mOperationMode);

        // Operation Mode BOOST overrides target temperature
        if (*mOperationMode == message::HMIOperationMode::OM_BOOST)
        {
            message.setWaterTempTarget(62.0);
        }
        else if (*mOperationMode == message::HMIOperationMode::OM_ABSENCE)
        {
            message.setWaterTempTarget(20.0);
        }
    }

    // if time has been set by rtc / ntp, override time and date in hmi message
    if (aquamqtt::config::OVERRIDE_TIME_AND_DATE_IN_MITM && mTimeIsSet)
    {
        message.setTimeHours(mTimeHours);
        message.setTimeMinutes(mTimeMinutes);
        message.setTimeSeconds(mTimeSeconds);
        message.setDateDay(mTimeDays);
        message.setDateMonthAndYear(mTimeMonth, mTimeYear);
    }

    xSemaphoreGive(mMutex);
}

bool HMIStateProxy::copyFrame(uint8_t frameId, uint8_t* buffer)
{
    if (frameId != aquamqtt::message::HMI_MESSAGE_IDENTIFIER)
    {
        return aquamqtt::DHWState::getInstance().copyFrame(frameId, buffer);
    }

    bool hasHmiMessage = aquamqtt::DHWState::getInstance().copyFrame(frameId, buffer);
    if (hasHmiMessage && aquamqtt::config::OPERATION_MODE == config::EOperationMode::MITM)
    {
        applyHMIOverrides(buffer);
    }
    return hasHmiMessage;
}

void HMIStateProxy::onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mOperationMode = std::move(value);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}

void HMIStateProxy::onWaterTempTargetChanged(std::unique_ptr<float> value)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mTargetTemperature = std::move(value);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}

void HMIStateProxy::onResetOverrides()
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mOperationMode     = std::unique_ptr<message::HMIOperationMode>(nullptr);
    mTargetTemperature = std::unique_ptr<float>(nullptr);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}
AquaMqttOverrides HMIStateProxy::getOverrides()
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return AquaMqttOverrides{};
    }

    auto retVal = AquaMqttOverrides{ mOperationMode != nullptr, mTargetTemperature != nullptr };

    xSemaphoreGive(mMutex);

    return retVal;
}
void HMIStateProxy::updateTime(
        uint8_t  seconds,
        uint8_t  minutes,
        uint8_t  hours,
        uint8_t  days,
        uint8_t  month,
        uint16_t year)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mTimeSeconds = seconds;
    mTimeMinutes = minutes;
    mTimeHours   = hours;
    mTimeDays    = days;
    mTimeMonth   = month;
    mTimeYear    = year;
    mTimeIsSet   = true;

    xSemaphoreGive(mMutex);
}

}  // namespace aquamqtt