#include "state/MainStateProxy.h"

#include "config/Configuration.h"
#include "message/IMainMessage.h"
#include "message/legacy/MainStatusMessage.h"
#include "message/next/MainStatusMessage.h"

namespace aquamqtt
{

MainStateProxy& MainStateProxy::getInstance()
{
    static MainStateProxy instance;
    return instance;
}

MainStateProxy::MainStateProxy()
    : IMQTTCallback()
    , mMutex(xSemaphoreCreateMutex())
    , mNotify(nullptr)
    , mPVModeHeatPump(false)
    , mPVModeHeatElement(false)
{
}

void MainStateProxy::setListener(TaskHandle_t handle)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mNotify = handle;

    xSemaphoreGive(mMutex);
}

void MainStateProxy::applyMainOverrides(uint8_t* buffer, message::ProtocolVersion& version)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    std::unique_ptr<message::IMainMessage> message;
    if(version == message::PROTOCOL_LEGACY){
        message = std::make_unique<message::legacy::MainStatusMessage>(buffer);
    } else{
        message = std::make_unique<message::next::MainStatusMessage>(buffer);
    }

    // we only want to modify the message if a custom pv mode is active -> else leave state as it is
    if (mPVModeHeatElement)
    {
        // let the hmi show the pv icon in case the heat element pv state is enabled
        message->setAttr(message::MAIN_ATTR_BOOL::STATE_PV, true);
    }

    if (mPVModeHeatPump)
    {
        // let the hmi show the solar icon in case the heat element pv state is enabled
        message->setAttr(message::MAIN_ATTR_BOOL::STATE_SOLAR, true);
    }

    xSemaphoreGive(mMutex);
}

size_t MainStateProxy::copyFrame(uint8_t frameId, uint8_t* buffer, message::ProtocolVersion& version)
{
    if (frameId != aquamqtt::message::MAIN_MESSAGE_IDENTIFIER)
    {
        return aquamqtt::DHWState::getInstance().copyFrame(frameId, buffer, version);
    }

    size_t mainMessageLength = aquamqtt::DHWState::getInstance().copyFrame(frameId, buffer, version);
    if (mainMessageLength > 0 && aquamqtt::config::OPERATION_MODE == config::EOperationMode::MITM)
    {
        applyMainOverrides(buffer, version);
    }
    return mainMessageLength;
}

void MainStateProxy::onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value)
{
    // noop
}

void MainStateProxy::onWaterTempTargetChanged(std::unique_ptr<float> value)
{
    // noop
}

void MainStateProxy::onHeatingElementEnabledChanged(std::unique_ptr<bool> enabled)
{
    // noop
}

void MainStateProxy::onEmergencyModeEnabledChanged(std::unique_ptr<bool> enabled)
{
    // noop
}

void MainStateProxy::onPVModeHeatpumpEnabled(bool enabled)
{

    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mPVModeHeatPump = enabled;

    // message 193 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 7UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}

void MainStateProxy::onPVModeHeatElementEnabled(bool enabled)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mPVModeHeatElement = enabled;

    // message 193 has changed
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, (1UL << 7UL), eSetBits);
    }

    xSemaphoreGive(mMutex);
}
void MainStateProxy::onFanExhaustModeChanged(std::unique_ptr<message::HMIFanExhaust> mode)
{
    // noop
}
void MainStateProxy::onAirductConfigChanged(std::unique_ptr<message::HMIAirDuctConfig> config)
{
    // noop
}

void MainStateProxy::onOperationTypeChanged(std::unique_ptr<message::HMIOperationType> type)
{
    // noop
}

void MainStateProxy::onInstallationModeChanged(std::unique_ptr<message::HMIInstallation> mode)
{
    // noop
}

void MainStateProxy::onResetOverrides()
{
    // noop
}
AquaMqttMainOverrides MainStateProxy::getOverrides()
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return AquaMqttMainOverrides{};
    }

    AquaMqttMainOverrides retVal{};

    if (mPVModeHeatPump)
    {
        retVal.solarState = true;
    }

    if (mPVModeHeatElement)
    {
        retVal.pvState = true;
    }

    xSemaphoreGive(mMutex);

    return retVal;
}

}  // namespace aquamqtt