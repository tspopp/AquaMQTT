#ifndef AQUAMQTT_HMISTATEPROXY_H
#define AQUAMQTT_HMISTATEPROXY_H

#include "message/HMIMessage.h"
#include "mqtt/IMQTTCallback.h"
#include "state/DHWState.h"

namespace aquamqtt
{

struct AquaMqttOverrides
{
    bool operationMode;
    bool waterTempTarget;
};

/**
 * HMIStateProxy accesses current HMI message from the DHW state
 * and applies overrides depending on the AquaMQTT Operation Mode
 * and overrides set using mqtt control topics
 */

class HMIStateProxy : public mqtt::IMQTTCallback
{
public:
    static HMIStateProxy& getInstance();

    virtual ~HMIStateProxy() = default;

    HMIStateProxy(const HMIStateProxy&) = delete;

private:
    HMIStateProxy();

public:
    HMIStateProxy& operator=(const HMIStateProxy&) = delete;

    void setListener(TaskHandle_t handle);

    void applyHMIOverrides(uint8_t* buffer);

    bool copyFrame(uint8_t frameId, uint8_t* buffer);

    void onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value) override;

    void onWaterTempTargetChanged(std::unique_ptr<float> value) override;

    void onResetOverrides() override;

    AquaMqttOverrides getOverrides();

    void updateTime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t days, uint8_t month, uint16_t year);

private:
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
    std::unique_ptr<message::HMIOperationMode> mOperationMode;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_HMISTATEPROXY_H
