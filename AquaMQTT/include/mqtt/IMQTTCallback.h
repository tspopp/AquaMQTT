#ifndef AQUAMQTT_IMQTTCALLBACK_H
#define AQUAMQTT_IMQTTCALLBACK_H

#include <memory>
namespace aquamqtt
{
namespace mqtt
{

class IMQTTCallback
{
public:
    // $prefix/aquamqtt/ctrl/operationMode + ENUM_OPERATION_MODE
    virtual void onOperationModeChanged(std::unique_ptr<message::HMIOperationMode> value) = 0;

    // $prefix/aquamqtt/ctrl/waterTempTarget + float
    virtual void onWaterTempTargetChanged(std::unique_ptr<float> value) = 0;

    // $prefix/aquamqtt/ctrl/heatingElementEnabled + bool
    virtual void onHeatingElementEnabledChanged(std::unique_ptr<bool> enabled) = 0;

    // $prefix/aquamqtt/ctrl/emergencyModeEnabled + bool
    virtual void onEmergencyModeEnabledChanged(std::unique_ptr<bool> enabled) = 0;

    // $prefix/aquamqtt/ctrl/operationType + ENUM OPERATION TYPE
    virtual void onOperationTypeChanged(std::unique_ptr<message::HMIOperationType> type) = 0;

    // $prefix/aquamqtt/ctrl/configInstallation + ENUM INSTALLATION
    virtual void onInstallationModeChanged(std::unique_ptr<message::HMIInstallation> mode) = 0;

    // $prefic/aquamqtt/ctrl/flagPVModeHeatPump + bool
    virtual void onPVModeHeatpumpEnabled(bool enabled) = 0;

    // $prefic/aquamqtt/ctrl/flagPVModeHeatElement + bool
    virtual void onPVModeHeatElementEnabled(bool enabled) = 0;

    // $prefix/aquamqtt/ctrl/reset
    virtual void onResetOverrides() = 0;
};
};  // namespace mqtt
}  // namespace aquamqtt
#endif  // AQUAMQTT_IMQTTCALLBACK_H
