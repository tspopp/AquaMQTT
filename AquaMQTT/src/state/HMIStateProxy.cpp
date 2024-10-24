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
    , mOperationType(nullptr)
    , mTargetTemperature(nullptr)
    , mTimeIsSet(false)
    , mPVModeHeatPump(false)
    , mPVModeHeatElement(false)
    , mEmergencyModeEnabled(nullptr)
    , mHeatingElementEnabled(nullptr)
    , mInstallationMode(nullptr)
    , mFanExhaustMode(nullptr)
    , mAirductConfig(nullptr)
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

    switch (currentOverrideMode())
    {
        case AM_MODE_PV_HP_ONLY:
            message.setInstallationMode(message::HMIInstallation::INST_HP_ONLY);
            message.setOperationType(message::HMIOperationType::ALWAYS_ON);
            message.setOperationMode(message::HMIOperationMode::OM_ECO_INACTIVE);
            message.setWaterTempTarget(config::MAX_WATER_TEMPERATURE);
            // do not use heat element
            message.setEmergencyMode(false);
            message.enableHeatingElement(false);
            break;
        case AM_MODE_PV_HE_ONLY:
            message.setInstallationMode(message::HMIInstallation::INST_HP_ONLY);
            message.setOperationType(message::HMIOperationType::ALWAYS_ON);
            message.setOperationMode(message::HMIOperationMode::OM_ECO_INACTIVE);
            message.setWaterTempTarget(config::MAX_WATER_TEMPERATURE);
            // just use heat element
            message.enableHeatingElement(true);
            message.setEmergencyMode(true);
            break;
        case AM_MODE_PV_FULL:
            message.setInstallationMode(message::HMIInstallation::INST_HP_ONLY);
            message.setOperationType(message::HMIOperationType::ALWAYS_ON);
            message.setOperationMode(message::HMIOperationMode::OM_BOOST);
            message.setWaterTempTarget(config::MAX_WATER_TEMPERATURE);
            break;
        case AM_MODE_STANDARD:
        {
            // TODO: this sanity should be handled within message
            if (mTargetTemperature != nullptr && *mTargetTemperature <= config::MAX_WATER_TEMPERATURE
                && *mTargetTemperature >= config::ABSENCE_WATER_TEMPERATURE)
            {
                message.setWaterTempTarget(*mTargetTemperature);
            }

            if (mInstallationMode != nullptr)
            {
                message.setInstallationMode(*mInstallationMode);
            }

            if (mFanExhaustMode != nullptr)
            {
                message.setFanExhaustMode(*mFanExhaustMode);
            }

            if (mAirductConfig != nullptr)
            {
                message.setAirDuctConfig(*mAirductConfig);
            }

            if (mOperationMode != nullptr)
            {
                message.setOperationMode(*mOperationMode);

                // Operation Mode BOOST overrides target temperature
                if (*mOperationMode == message::HMIOperationMode::OM_BOOST)
                {
                    message.setWaterTempTarget(config::MAX_WATER_TEMPERATURE);
                }
                else if (*mOperationMode == message::HMIOperationMode::OM_ABSENCE)
                {
                    message.setWaterTempTarget(config::ABSENCE_WATER_TEMPERATURE);
                }
            }

            if (mOperationType != nullptr)
            {
                message.setOperationType(*mOperationType);
            }

            if (mHeatingElementEnabled != nullptr)
            {
                // sanity is taken care within message
                message.enableHeatingElement(*mHeatingElementEnabled);
            }

            if (mEmergencyModeEnabled != nullptr)
            {
                // sanity is taken care within message
                message.setEmergencyMode(*mEmergencyModeEnabled);
            }
        }
        break;
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

void HMIStateProxy::onHeatingElementEnabledChanged(std::unique_ptr<bool> enabled)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mHeatingElementEnabled = std::move(enabled);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}

void HMIStateProxy::onEmergencyModeEnabledChanged(std::unique_ptr<bool> enabled)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mEmergencyModeEnabled = std::move(enabled);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}
void HMIStateProxy::onFanExhaustModeChanged(std::unique_ptr<message::HMIFanExhaust> mode)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mFanExhaustMode = std::move(mode);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}
void HMIStateProxy::onAirductConfigChanged(std::unique_ptr<message::HMIAirDuctConfig> config)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mAirductConfig = std::move(config);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}

void HMIStateProxy::onPVModeHeatpumpEnabled(bool enabled)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mPVModeHeatPump = enabled;

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}

void HMIStateProxy::onPVModeHeatElementEnabled(bool enabled)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mPVModeHeatElement = enabled;

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

    mOperationMode         = std::unique_ptr<message::HMIOperationMode>(nullptr);
    mTargetTemperature     = std::unique_ptr<float>(nullptr);
    mOperationType         = std::unique_ptr<message::HMIOperationType>(nullptr);
    mInstallationMode      = std::unique_ptr<message::HMIInstallation>(nullptr);
    mFanExhaustMode        = std::unique_ptr<message::HMIFanExhaust>(nullptr);
    mAirductConfig         = std::unique_ptr<message::HMIAirDuctConfig>(nullptr);
    mEmergencyModeEnabled  = std::unique_ptr<bool>(nullptr);
    mHeatingElementEnabled = std::unique_ptr<bool>(nullptr);

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

    AquaMqttOverrides retVal{};
    switch (currentOverrideMode())
    {
        case AM_MODE_STANDARD:
            retVal = AquaMqttOverrides{ mOperationMode != nullptr,        mOperationType != nullptr,
                                        mTargetTemperature != nullptr,    mHeatingElementEnabled != nullptr,
                                        mEmergencyModeEnabled != nullptr, mInstallationMode != nullptr,
                                        mFanExhaustMode != nullptr,       mAirductConfig != nullptr };
            break;
        case AM_MODE_PV_HP_ONLY:
        case AM_MODE_PV_HE_ONLY:
            retVal = AquaMqttOverrides{ true, true, true, true, true, true, false, false };
            break;
        case AM_MODE_PV_FULL:
            retVal = AquaMqttOverrides{ true, true, true, false, false, true, false, false };
            break;
    }

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

AquaMqttOverrideMode HMIStateProxy::currentOverrideMode() const
{
    if (mPVModeHeatElement && mPVModeHeatPump)
    {
        return AM_MODE_PV_FULL;
    }
    else if (!mPVModeHeatElement && mPVModeHeatPump)
    {
        return AM_MODE_PV_HP_ONLY;
    }
    else if (mPVModeHeatElement && !mPVModeHeatPump)
    {
        return AM_MODE_PV_HE_ONLY;
    }
    return AM_MODE_STANDARD;
}

void HMIStateProxy::onOperationTypeChanged(std::unique_ptr<message::HMIOperationType> type)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mOperationType = std::move(type);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}

void HMIStateProxy::onInstallationModeChanged(std::unique_ptr<message::HMIInstallation> mode)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mInstallationMode = std::move(mode);

    // message 194 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}

AquaMqttOverrideMode HMIStateProxy::getOverrideMode()
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return AM_MODE_STANDARD;
    }

    auto retVal = currentOverrideMode();

    xSemaphoreGive(mMutex);

    return retVal;
}

bool HMIStateProxy::isPVModeHeatPumpEnabled()
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return false;
    }

    auto retVal = mPVModeHeatPump;

    xSemaphoreGive(mMutex);

    return retVal;
}

bool HMIStateProxy::isPVModeHeatElementEnabled()
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return false;
    }

    auto retVal = mPVModeHeatElement;

    xSemaphoreGive(mMutex);

    return retVal;
}

}  // namespace aquamqtt