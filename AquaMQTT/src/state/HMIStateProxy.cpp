#include "state/HMIStateProxy.h"

#include "config/Configuration.h"
#include "message/Factory.h"

namespace aquamqtt
{

HMIStateProxy& HMIStateProxy::getInstance()
{
    static HMIStateProxy instance;
    return instance;
}

HMIStateProxy::HMIStateProxy()
    : IMQTTCallback()
    , mNotify(nullptr)
    , mMutex(xSemaphoreCreateMutex())
    , mTimeIsSet(false)
    , mTargetTemperature(nullptr)
    , mOperationType(nullptr)
    , mOperationMode(nullptr)
    , mInstallationMode(nullptr)
    , mFanExhaustMode(nullptr)
    , mAirductConfig(nullptr)
    , mEmergencyModeEnabled(nullptr)
    , mHeatingElementEnabled(nullptr)
    , mPVModeHeatPump(false)
    , mPVModeHeatElement(false)
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

void HMIStateProxy::applyHMIOverrides(uint8_t* buffer, const message::ProtocolVersion& version) const
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    const std::unique_ptr<message::IHMIMessage> message = createHmiMessageFromBuffer(version, buffer);

    switch (currentOverrideMode())
    {
        case AM_MODE_PV_HP_ONLY:
            message->setAttr(message::HMI_ATTR_U8::STATE_INSTALLATION_MODE, message::HMIInstallation::INST_HP_ONLY);
            message->setAttr(message::HMI_ATTR_U8::OPERATION_TYPE, message::HMIOperationType::OT_ALWAYS_ON);
            if (version == message::PROTOCOL_ODYSSEE) {
                // Odyssee does not have the option to disable the heat element. Therefore, we enter operation mode
                // ECO ACTIVE (which forbids usage of heat element) and set the target temperature to maximum temperature
                // This configuration is not allowed in the HMI controller, but we hope the Main controller accepts it :)
                message->setAttr(message::HMI_ATTR_U8::OPERATION_MODE, message::HMIOperationMode::OM_ECO_ACTIVE);
            } else {
                message->setAttr(message::HMI_ATTR_U8::OPERATION_MODE, message::HMIOperationMode::OM_ECO_INACTIVE);
            }
            message->setAttr(message::HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE, config::MAX_WATER_TEMPERATURE);
            // do not use heat element
            message->setAttr(message::HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED, false);
            message->setAttr(message::HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED, false);
            break;
        case AM_MODE_PV_HE_ONLY:
            message->setAttr(message::HMI_ATTR_U8::STATE_INSTALLATION_MODE, message::HMIInstallation::INST_HP_ONLY);
            message->setAttr(message::HMI_ATTR_U8::OPERATION_TYPE, message::HMIOperationType::OT_ALWAYS_ON);
            message->setAttr(message::HMI_ATTR_U8::OPERATION_MODE, message::HMIOperationMode::OM_ECO_INACTIVE);
            message->setAttr(message::HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE, config::MAX_WATER_TEMPERATURE);
            // just use heat element
            message->setAttr(message::HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED, true);
            message->setAttr(message::HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED, true);
            break;
        case AM_MODE_PV_FULL:
            message->setAttr(message::HMI_ATTR_U8::STATE_INSTALLATION_MODE, message::HMIInstallation::INST_HP_ONLY);
            message->setAttr(message::HMI_ATTR_U8::OPERATION_TYPE, message::HMIOperationType::OT_ALWAYS_ON);
            message->setAttr(message::HMI_ATTR_U8::OPERATION_MODE, message::HMIOperationMode::OM_BOOST);
            message->setAttr(message::HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE, config::MAX_WATER_TEMPERATURE);
            break;
        case AM_MODE_STANDARD:
        {
            // TODO: this sanity should be handled within message
            if (mTargetTemperature != nullptr && *mTargetTemperature <= config::MAX_WATER_TEMPERATURE
                && *mTargetTemperature >= config::ABSENCE_WATER_TEMPERATURE)
            {
                message->setAttr(message::HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE, *mTargetTemperature);
            }

            if (mInstallationMode != nullptr)
            {
                message->setAttr(message::HMI_ATTR_U8::STATE_INSTALLATION_MODE, *mInstallationMode);
            }

            if (mFanExhaustMode != nullptr)
            {
                message->setAttr(message::HMI_ATTR_U8::CONFIG_FAN_EXHAUST, *mFanExhaustMode);
            }

            if (mAirductConfig != nullptr)
            {
                message->setAttr(message::HMI_ATTR_U8::CONFIG_AIRDUCT, *mAirductConfig);
            }

            if (mOperationMode != nullptr)
            {
                message->setAttr(message::HMI_ATTR_U8::OPERATION_MODE, *mOperationMode);

                // Operation Mode BOOST overrides target temperature
                if (*mOperationMode == message::HMIOperationMode::OM_BOOST)
                {
                    message->setAttr(message::HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE, config::MAX_WATER_TEMPERATURE);
                }
                else if (*mOperationMode == message::HMIOperationMode::OM_ABSENCE)
                {
                    message->setAttr(
                            message::HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE,
                            config::ABSENCE_WATER_TEMPERATURE);
                }
            }

            if (mOperationType != nullptr)
            {
                message->setAttr(message::HMI_ATTR_U8::OPERATION_TYPE, *mOperationType);
            }

            if (mHeatingElementEnabled != nullptr)
            {
                // sanity is taken care within message
                message->setAttr(message::HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED, *mHeatingElementEnabled);
            }

            if (mEmergencyModeEnabled != nullptr)
            {
                // sanity is taken care within message
                message->setAttr(message::HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED, *mEmergencyModeEnabled);
            }
        }
        break;
    }

    // if time has been set by rtc / ntp, override time and date in hmi message
    if (aquamqtt::config::OVERRIDE_TIME_AND_DATE_IN_MITM && mTimeIsSet)
    {
        message->setAttr(message::HMI_ATTR_U8::TIME_HOURS, mTimeHours);
        message->setAttr(message::HMI_ATTR_U8::TIME_MINUTES, mTimeMinutes);
        message->setAttr(message::HMI_ATTR_U8::TIME_SECONDS, mTimeSeconds);
        message->setAttr(message::HMI_ATTR_U8::DATE_DAY, mTimeDays);
        message->setDateMonthAndYear(mTimeMonth, mTimeYear);
    }

    xSemaphoreGive(mMutex);
}

size_t HMIStateProxy::copyFrame(
        const uint8_t              frameId,
        uint8_t*                   buffer,
        message::ProtocolVersion&  version,
        message::ProtocolChecksum& type)
{
    if (frameId != aquamqtt::message::HMI_MESSAGE_IDENTIFIER)
    {
        return aquamqtt::DHWState::getInstance().copyFrame(frameId, buffer, version, type);
    }

    size_t hmiMessageLength = aquamqtt::DHWState::getInstance().copyFrame(frameId, buffer, version, type);
    if (hmiMessageLength > 0 && aquamqtt::config::OPERATION_MODE == config::EOperationMode::MITM)
    {
        applyHMIOverrides(buffer, version);
    }
    return hmiMessageLength;
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

AquaMqttOverrides HMIStateProxy::getOverrides() const
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
        const uint8_t  seconds,
        const uint8_t  minutes,
        const uint8_t  hours,
        const uint8_t  days,
        const uint8_t  month,
        const uint16_t year)
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
    if (!mPVModeHeatElement && mPVModeHeatPump)
    {
        return AM_MODE_PV_HP_ONLY;
    }
    if (mPVModeHeatElement && !mPVModeHeatPump)
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

AquaMqttOverrideMode HMIStateProxy::getOverrideMode() const
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return AM_MODE_STANDARD;
    }

    const auto retVal = currentOverrideMode();

    xSemaphoreGive(mMutex);

    return retVal;
}

bool HMIStateProxy::isPVModeHeatPumpEnabled() const
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return false;
    }

    const auto retVal = mPVModeHeatPump;

    xSemaphoreGive(mMutex);

    return retVal;
}

bool HMIStateProxy::isPVModeHeatElementEnabled() const
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return false;
    }

    const auto retVal = mPVModeHeatElement;

    xSemaphoreGive(mMutex);

    return retVal;
}

}  // namespace aquamqtt