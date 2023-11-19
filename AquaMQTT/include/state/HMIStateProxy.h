#ifndef AQUAMQTT_HMISTATEPROXY_H
#define AQUAMQTT_HMISTATEPROXY_H

#include <Arduino.h>

#include "TimeLib.h"
#include "config/Modes.h"
#include "message/HMIMessage.h"
#include "mqtt/IMQTTCallback.h"
#include "state/DHWState.h"

namespace aquamqtt
{

/**
 * HMIStateProxy accesses current HMI message from the DHW state
 * and applies overrides depending on the AquaMQTT Operation Mode
 * and overrides set using mqtt control topics
 */

class HMIStateProxy : public IMQTTCallback
{
public:
    static HMIStateProxy& getInstance()
    {
        static HMIStateProxy instance;
        return instance;
    }

    virtual ~HMIStateProxy() = default;

    HMIStateProxy(const HMIStateProxy&) = delete;

private:
    HMIStateProxy()
        : IMQTTCallback()
        , mMutex(xSemaphoreCreateMutex())
        , mNotify(nullptr)
        , mOperationMode(nullptr)
        , mTargetTemperature(nullptr)
        , mTimerModeEnabled(nullptr){};
    HMIStateProxy& operator=(const HMIStateProxy&) = delete;

public:
    void setListener(TaskHandle_t handle)
    {
        if (!xSemaphoreTake(mMutex, portMAX_DELAY))
        {
            return;
        }

        mNotify = handle;

        xSemaphoreGive(mMutex);
    }

    void applyHMIOverrides(uint8_t* buffer)
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
            if (*mOperationMode == message::HMIOperationMode::BOOST)
            {
                message.setWaterTempTarget(62.0);
            }
            else if (*mOperationMode == message::HMIOperationMode::ABSENCE)
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

    bool copyFrame(uint8_t frameId, uint8_t* buffer)
    {
        if (frameId != 194)
        {
            return aquamqtt::DHWState::getInstance().copyFrame(frameId, buffer);
        }

        bool has194 = aquamqtt::DHWState::getInstance().copyFrame(frameId, buffer);
        if (has194 && aquamqtt::config::OPERATION_MODE == EOperationMode::MITM)
        {
            applyHMIOverrides(buffer);
        }
        return has194;
    }

    void onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value) override
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

    void onWaterTempTargetChanged(std::unique_ptr<float> value) override
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

private:
    TaskHandle_t      mNotify;
    SemaphoreHandle_t mMutex;

    // since we do not have optionals, use nullptr if override is not set
    std::unique_ptr<float>            mTargetTemperature;
    std::unique_ptr<message::HMIOperationMode> mOperationMode;
    std::unique_ptr<bool>             mTimerModeEnabled;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_HMISTATEPROXY_H
