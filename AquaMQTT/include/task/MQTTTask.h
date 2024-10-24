#ifndef AQUAMQTT_MQTTTASK_H
#define AQUAMQTT_MQTTTASK_H

#include <MQTT.h>
#include <WiFiClient.h>

#include "SimpleKalmanFilter.h"
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

    virtual ~MQTTTask();

    void spawn();

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    void setup();

    void loop();

    void check_mqtt_connection();

    static void messageReceived(String& topic, String& payload);

    void updateMainStatus(bool triggerFullUpdate);
    void updateHMIStatus(bool triggerFullUpdate);
    void updateEnergyStats(bool triggerFullUpdate);
    void updateErrorStatus();
    void updateStats();

private:
    uint8_t       mTransferBuffer[message::HEATPUMP_MAX_FRAME_LENGTH];
    uint8_t       mTopicBuffer[config::MQTT_MAX_TOPIC_SIZE];
    uint8_t       mPayloadBuffer[config::MQTT_MAX_PAYLOAD_SIZE];
    unsigned long mLastStatsUpdate;
    unsigned long mLastFullUpdate;
    WiFiClient    mWiFiClient;
    MQTTClient    mMQTTClient;
    TaskHandle_t  mTaskHandle;

    uint8_t* mLastProcessedHMIMessage;
    uint8_t* mLastProcessedEnergyMessage;
    uint8_t* mLastProcessedMainMessage;

    SimpleKalmanFilter mEvaporatorLowerAirTempFilter;
    SimpleKalmanFilter mEvaporatorUpperAirTempFilter;
    SimpleKalmanFilter mAirTempFilter;
    SimpleKalmanFilter mHotWaterTempFilter;

    // helper to avoid code duplication
    void publishFloat(const char* subtopic, const char* topic, float value, bool retained = false);
    void publishString(const char* subtopic, const char* topic, const char* value, bool retained = false);
    void publishi(const char* subtopic, const char* topic, int value, bool retained = false);
    void publishul(const char* subtopic, const char* topic, unsigned long value, bool retained = false);
    void publishul(
            const char*   subtopic_1,
            const char*   subtopic_2,
            const char*   topic,
            unsigned long value,
            bool          retained = false);
    void applyTemperatureFilter(message::MainStatusMessage* pMessage);

    void enableDiscovery();
    template <typename T>
    void publishDiscovery(uint16_t identifier, const char* haCategory, T enumClass);
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_MQTTTASK_H
