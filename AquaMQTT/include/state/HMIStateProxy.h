#ifndef AQUAMQTT_HMISTATEPROXY_H
#define AQUAMQTT_HMISTATEPROXY_H

#include "mqtt/IMQTTCallback.h"
#include "state/DHWState.h"

namespace aquamqtt
{

struct AquaMqttOverrides
{
    bool operationMode;
    bool operationType;
    bool waterTempTarget;
    bool heatingElementEnabled;
    bool emergencyModeEnabled;
    bool installationMode;
    bool exhaustFanMode;
    bool airductConfig;
};

enum AquaMqttOverrideMode
{
    AM_MODE_STANDARD   = 0,
    AM_MODE_PV_HP_ONLY = 1,
    AM_MODE_PV_HE_ONLY = 2,
    AM_MODE_PV_FULL    = 3
};

static const char* aquamqttOverrideStr(AquaMqttOverrideMode mode)
{
    switch (mode)
    {
        case AM_MODE_STANDARD:
            return reinterpret_cast<const char*>(mqtt::ENUM_AQUAMQTT_OVERRIDE_MODE_DEFAULT);
        case AM_MODE_PV_HP_ONLY:
            return reinterpret_cast<const char*>(mqtt::ENUM_AQUAMQTT_OVERRIDE_MODE_HP_ONLY);
        case AM_MODE_PV_HE_ONLY:
            return reinterpret_cast<const char*>(mqtt::ENUM_AQUAMQTT_OVERRIDE_MODE_HE_ONLY);
        case AM_MODE_PV_FULL:
            return reinterpret_cast<const char*>(mqtt::ENUM_AQUAMQTT_OVERRIDE_MODE_PV_FULL);
        default:
            return reinterpret_cast<const char*>(mqtt::ENUM_UNKNOWN);
    }
}

/**
 * HMIStateProxy accesses current HMI message from the DHW state
 * and applies overrides depending on the AquaMQTT Operation Mode
 * and overrides set using mqtt control topics
 */

class HMIStateProxy : public mqtt::IMQTTCallback
{
public:
    static HMIStateProxy& getInstance();

    ~HMIStateProxy() override = default;

    HMIStateProxy(const HMIStateProxy&) = delete;

private:
    HMIStateProxy();

public:
    HMIStateProxy& operator=(const HMIStateProxy&) = delete;

    void setListener(TaskHandle_t handle);

    void applyHMIOverrides(uint8_t* buffer, message::ProtocolVersion& version);

    size_t copyFrame(
            uint8_t                    frameId,
            uint8_t*                   buffer,
            message::ProtocolVersion&  version,
            message::ProtocolChecksum& type);

    void onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value) override;

    void onOperationTypeChanged(std::unique_ptr<message::HMIOperationType> type) override;

    void onInstallationModeChanged(std::unique_ptr<message::HMIInstallation> mode) override;

    void onWaterTempTargetChanged(std::unique_ptr<float> value) override;

    void onHeatingElementEnabledChanged(std::unique_ptr<bool> enabled) override;

    void onEmergencyModeEnabledChanged(std::unique_ptr<bool> enabled) override;

    void onFanExhaustModeChanged(std::unique_ptr<message::HMIFanExhaust> mode) override;

    void onAirductConfigChanged(std::unique_ptr<message::HMIAirDuctConfig> config) override;

    void onPVModeHeatpumpEnabled(bool enabled) override;

    bool isPVModeHeatPumpEnabled();

    void onPVModeHeatElementEnabled(bool enabled) override;

    bool isPVModeHeatElementEnabled();

    void onResetOverrides() override;

    AquaMqttOverrides getOverrides();

    AquaMqttOverrideMode getOverrideMode();

    void updateTime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t days, uint8_t month, uint16_t year);

private:
    AquaMqttOverrideMode currentOverrideMode() const;

    TaskHandle_t      mNotify;
    SemaphoreHandle_t mMutex;
    bool              mTimeIsSet;
    uint8_t           mTimeSeconds;
    uint8_t           mTimeMinutes;
    uint8_t           mTimeHours;
    uint8_t           mTimeDays;
    uint8_t           mTimeMonth;
    uint16_t          mTimeYear;

    // since we do not have optionals, use nullptr if override is not set
    std::unique_ptr<float>                     mTargetTemperature;
    std::unique_ptr<message::HMIOperationType> mOperationType;
    std::unique_ptr<message::HMIOperationMode> mOperationMode;
    std::unique_ptr<message::HMIInstallation>  mInstallationMode;
    std::unique_ptr<message::HMIFanExhaust>    mFanExhaustMode;
    std::unique_ptr<message::HMIAirDuctConfig> mAirductConfig;
    std::unique_ptr<bool>                      mEmergencyModeEnabled;
    std::unique_ptr<bool>                      mHeatingElementEnabled;
    bool                                       mPVModeHeatPump;
    bool                                       mPVModeHeatElement;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_HMISTATEPROXY_H
