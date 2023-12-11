#ifndef AQUAMQTT_IMQTTCALLBACK_H
#define AQUAMQTT_IMQTTCALLBACK_H

#include <memory>
namespace aquamqtt
{

class IMQTTCallback
{
public:
    // $prefix/aquamqtt/ctrl/operationMode + ENUM_OPERATION_MODE
    virtual void onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value) = 0;

    // $prefix/aquamqtt/ctrl/waterTempTarget + float
    virtual void onWaterTempTargetChanged(std::unique_ptr<float> value) = 0;

    // $prefix/aquamqtt/ctrl/reset
    virtual void onResetOverrides() = 0;
};
}  // namespace aquamqtt
#endif  // AQUAMQTT_IMQTTCALLBACK_H
