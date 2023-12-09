#ifndef AQUAMQTT_HMISTATEPROXY_H
#define AQUAMQTT_HMISTATEPROXY_H

#include <Arduino.h>

#include "TimeLib.h"
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
    static HMIStateProxy& getInstance();

    virtual ~HMIStateProxy() = default;

    HMIStateProxy(const HMIStateProxy&) = delete;

private:
    HMIStateProxy();;

public:
    HMIStateProxy& operator=(const HMIStateProxy&) = delete;

    void setListener(TaskHandle_t handle);

    void applyHMIOverrides(uint8_t* buffer);

    bool copyFrame(uint8_t frameId, uint8_t* buffer);

    void onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value) override;

    void onWaterTempTargetChanged(std::unique_ptr<float> value) override;

private:
    TaskHandle_t      mNotify;
    SemaphoreHandle_t mMutex;

    // since we do not have optionals, use nullptr if override is not set
    std::unique_ptr<float>                     mTargetTemperature;
    std::unique_ptr<message::HMIOperationMode> mOperationMode;
    std::unique_ptr<bool>                      mTimerModeEnabled;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_HMISTATEPROXY_H
