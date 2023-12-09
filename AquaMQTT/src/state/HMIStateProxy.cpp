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
    , mTimerModeEnabled(nullptr)
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
    if (timeStatus() == timeSet)
    {
        message.setTimeHours(hour());
        message.setTimeMinutes(minute());
        message.setTimeSeconds(second());
        message.setDateDay(day());
        message.setDateMonthAndYear(month(), year());
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
    if (hasHmiMessage && aquamqtt::config::OPERATION_MODE == EOperationMode::MITM)
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
}  // namespace aquamqtt