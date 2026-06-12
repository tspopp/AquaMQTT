#ifndef AQUAMQTT_MQTTTASK_V5_H
#define AQUAMQTT_MQTTTASK_V5_H

#include <queue>

#include <FastCRC.h>
#include <WiFiClient.h>
#include <MQTTClient.h>

#include "Frame.h"
#include "dhwstate.h"
#include "Task.h"

#define DROPPED_SIZE 160

namespace aquamqtt
{

/**
 * Task to manage MQTT traffic:
 * 
 *   1. publish entities to Home Assistant
 *   2. send frames to MQTT for debugging
 * 
 * All communication with home assistant uses the 'homeassistant' topic.
 * 
 * Debug frames are published to the topic:
 * 
 *   DEVICE_ID/debug/CATEGORY/CHANNEL
 * 
 * where
 * 
 *   - DEVICE_ID is a unique ID of the AquaMQTT device (derived from the MAC address),
 *   - CATEGORY is either 'frame' (for valid frames) or 'dropped' (for invalid data),
 *   - CHANNEL is the serial communication channel (one of: 'listener', 'hmi' or 'main').
 */
class MqttTaskV5 final : public Task
{
private:
    const char* host;
    int port;

    bool discovery_is_published = false;

    SemaphoreHandle_t   queue_mutex;
    std::queue<Entity*> tainted_entities;
    std::queue<Frame>   frame_queue;
    std::queue<Frame>   dropped_queue;

    unsigned long       last_statistics_update_timestamp = 0;

    WiFiClient net;
    MQTTClient client;

public:
    static MqttTaskV5& getInstance();

    MqttTaskV5(MqttTaskV5 const&) = delete;
    void operator=(MqttTaskV5 const&) = delete;

    void queueUpdateEntity(Entity* entity);

    #ifdef MQTT_PUBLISH_FRAMES
    void queueFrame(const Frame& frame);
    void queueDroppedBytes(const Frame& frame);
    #endif

    void setup() override;
    void loop() override;

    void registerCommandTopic(Entity* entity);

private:
    std::map<String, Entity*> command_topics;
    static void messageReceived(const String& topic, const String& payload);

    MqttTaskV5(const char* host, int port);
    ~MqttTaskV5() = default;

    void connect();
    void publishEntityState(Entity &entity);
    void publishEntityDiscovery(Entity &entity);

    void publishDiscovery();
    void publishEntityStates();
    void publishFrames();
    void subscribe();
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_MQTTTASK_V5_H
