#ifndef AQUAMQTT_MQTTTASK_H
#define AQUAMQTT_MQTTTASK_H

#include <MQTT.h>
#include <WiFiClient.h>

#include "config/Configuration.h"
#include "message/HMIMessage.h"
#include "message/MainEnergyMessage.h"
#include "message/MainStatusMessage.h"
#include "message/MessageConstants.h"

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
    uint8_t       mTransferBuffer[message::HEATPUMP_MAX_FRAME_LENGTH];
    uint8_t       mTopicBuffer[config::MQTT_MAX_TOPIC_SIZE];
    uint8_t       mPayloadBuffer[config::MQTT_MAX_PAYLOAD_SIZE];
    unsigned long mLastFullUpdate;
    WiFiClient    mWiFiClient;
    MQTTClient    mMQTTClient;
    TaskHandle_t  mTaskHandle;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_MQTTTASK_H
