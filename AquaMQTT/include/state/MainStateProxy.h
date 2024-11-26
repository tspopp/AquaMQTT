#ifndef AQUAMQTT_MAINSTATEPROXY_H
#define AQUAMQTT_MAINSTATEPROXY_H

#include "mqtt/IMQTTCallback.h"
#include "state/DHWState.h"

namespace aquamqtt
{

struct AquaMqttMainOverrides
{
    bool pvState;
    bool solarState;
};

/**
 * MainStateProxy accesses messages emitted by the main controller through DHW
 * state and applies overrides for customizing the HMI
 */

class MainStateProxy : public mqtt::IMQTTCallback
{
public:
    static MainStateProxy& getInstance();

    virtual ~MainStateProxy() = default;

    MainStateProxy(const MainStateProxy&) = delete;

private:
    MainStateProxy();

public:
    MainStateProxy& operator=(const MainStateProxy&) = delete;

    void setListener(TaskHandle_t handle);

    void applyMainOverrides(uint8_t* buffer, message::ProtocolVersion& version);

    size_t copyFrame(uint8_t frameId, uint8_t* buffer, message::ProtocolVersion& version);

    void onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value) override;

    void onOperationTypeChanged(std::unique_ptr<message::HMIOperationType> type) override;

    void onInstallationModeChanged(std::unique_ptr<message::HMIInstallation> mode) override;

    void onWaterTempTargetChanged(std::unique_ptr<float> value) override;

    void onHeatingElementEnabledChanged(std::unique_ptr<bool> enabled) override;

    void onEmergencyModeEnabledChanged(std::unique_ptr<bool> enabled) override;

    void onPVModeHeatpumpEnabled(bool enabled) override;

    void onPVModeHeatElementEnabled(bool enabled) override;

    void onFanExhaustModeChanged(std::unique_ptr<message::HMIFanExhaust> mode) override;

    void onAirductConfigChanged(std::unique_ptr<message::HMIAirDuctConfig> config) override;

    void onResetOverrides() override;

    AquaMqttMainOverrides getOverrides();

private:
    TaskHandle_t      mNotify;
    SemaphoreHandle_t mMutex;

    bool mPVModeHeatPump;
    bool mPVModeHeatElement;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_MAINSTATEPROXY_H
