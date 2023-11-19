#ifndef AQUAMQTT_MQTTTASK_H
#define AQUAMQTT_MQTTTASK_H

#include <Arduino.h>
#include <MQTT.h>
#include <WiFiClient.h>

#include "message/HMIMessage.h"
#include "message/MainEnergyMessage.h"
#include "message/MainStatusMessage.h"

namespace aquamqtt
{
class MQTTTask
{
public:
    MQTTTask();

    virtual ~MQTTTask() = default;

    void spawn();

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    void setup();

    void loop();

    static void messageReceived(String& topic, String& payload);

    void updateMainStatus();
    void updateHMIStatus();
    void updateEnergyStats();
    void updateStats();

private:
    uint8_t       mTransferBuffer[40];
    unsigned long mLastFullUpdate;
    WiFiClient    mWiFiClient;
    MQTTClient    mMQTTClient;
    TaskHandle_t  mTaskHandle;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_MQTTTASK_H
