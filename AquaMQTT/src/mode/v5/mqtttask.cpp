#include "mqtttask.h"

#include <esp_task_wdt.h>
#include <sstream>

#include "config/Configuration.h"
#include "handler/Wifi.h"
#include "util.h"


namespace aquamqtt
{

/*** MQTTTASK PUBLIC ***/

MqttTaskV5& MqttTaskV5::getInstance() {
    static MqttTaskV5 instance(config::brokerAddr, config::brokerPort);
    return instance;
}

void MqttTaskV5::queueUpdateEntity(Entity* entity)
{
    if (xSemaphoreTake(queue_mutex, portMAX_DELAY)) {
        tainted_entities.push(entity);
        xSemaphoreGive(queue_mutex);
    }
}

#ifdef MQTT_PUBLISH_FRAMES
void MqttTaskV5::queueFrame(const Frame& frame) {
    if (xSemaphoreTake(queue_mutex, portMAX_DELAY)) {
        frame_queue.push(frame);
        xSemaphoreGive(queue_mutex);
    }
}

void MqttTaskV5::queueDroppedBytes(const Frame& frame) {
    /*
    char hex[MAX_FRAME_SIZE * 2 + 1];
    for (int i=0 ; i<frame.get_buffer_size() ; i++) {
        sprintf(hex + 2*i, "%02X", frame.get_buffer()[i]);
    }
    LOG.print("dropping: ");
    LOG.println(hex);
    */

    if (xSemaphoreTake(queue_mutex, portMAX_DELAY)) {
        dropped_queue.push(frame);
        xSemaphoreGive(queue_mutex);
    }
}
#endif

void MqttTaskV5::setup()
{
    Task::setup();

    client.begin(host, port, net);
    client.onMessage(MqttTaskV5::messageReceived);
}

void MqttTaskV5::loop()
{
    Task::loop();

    client.loop();

    if (!client.connected()) {
        connect();
        return;
    }

    if (!discovery_is_published) {
        publishDiscovery();
        subscribe();
        discovery_is_published = true;
    }

    publishEntityStates();

    #ifdef MQTT_PUBLISH_FRAMES
    publishFrames();
    #endif

    const bool print_stats = (millis() - last_statistics_update_timestamp) >= 5000;
}

void MqttTaskV5::registerCommandTopic(Entity* entity)
{
    command_topics[entity->getCommandTopic()] = entity;
    LOG.print("subscribe to topic: ");
    LOG.println(entity->getCommandTopic());
    client.subscribe(entity->getCommandTopic());
}

/*** MQTTTASK PRIVATE ***/

void MqttTaskV5::messageReceived(const String& topic, const String& payload)
{
    LOG.print("received mqtt: topic=");
    LOG.print(topic.c_str());
    LOG.print("; payload=");
    LOG.println(payload.c_str());
    if (MqttTaskV5::getInstance().command_topics.contains(topic)) {
        LOG.println("notifying entity");
        MqttTaskV5::getInstance().command_topics[topic]->set_state(payload.c_str());
    }
}

MqttTaskV5::MqttTaskV5(const char* _host, int _port)
    : Task("mqtt")
    , host(_host)
    , port(_port)
    , client(256)
{
    queue_mutex = xSemaphoreCreateMutex();
}

void MqttTaskV5::connect()
{
    if (!WifiHandler::getInstance().connected()) {
        return;
    }

    LOG.println("[mqtt] trying to connect");
    while (!client.connect(
            config::brokerClientId,
            strlen(config::brokerUser) == 0 ? nullptr : config::brokerUser,
            strlen(config::brokerPassword) == 0 ? nullptr : config::brokerPassword))
    {
        //vTaskDelay(pdMS_TO_TICKS(1000));
        delay(1000);
        esp_task_wdt_reset();
    }
    LOG.println("[mqtt] connected");

    discovery_is_published = false;
}

void MqttTaskV5::publishEntityState(Entity &entity)
{
    client.publish(entity.getStateTopic(), entity.state(), true, 0);
}

void MqttTaskV5::publishEntityDiscovery(Entity &entity)
{
    char topic[100];
    snprintf(topic, 100, "homeassistant/%s/%s/config", entity.getPlatform(), entity.getUniqueId());

    std::stringstream s;
    s << "{";

    // entity info
    entity.writeDefinition(s);

    // device info
    s << "\"dev\":{"; // device
    s << "\"ids\":\"" << entity.getDevice().getDeviceId() << "\""; // identifiers
    s << ",";
    s << "\"name\":\"" << entity.getDevice().getDeviceName() << "\"";
    s << "},";

    // origin info
    s << "\"o\":{";
    s << "\"name\":\"" << HA_ORIGIN_NAME << "\"";
    s << "}";

    s << "}";

    LOG.print("[mqtt] discovery ");
    LOG.print(topic);
    LOG.print(": ");
    LOG.println(s.str().c_str());

    client.publish(topic, s.str().c_str(), true, 0);
}

void MqttTaskV5::publishDiscovery() {
    DhwState& state = DhwState::getInstance();
    for (auto it = state.entities.begin() ; it != state.entities.end() ; it++) {
        publishEntityDiscovery(**it);
    }
}

void MqttTaskV5::publishEntityStates() {
    if (xSemaphoreTake(queue_mutex, portMAX_DELAY)) {
        while (!tainted_entities.empty()) {
            publishEntityState(*tainted_entities.front());
            tainted_entities.pop();
        }

        xSemaphoreGive(queue_mutex);
    }
}

#ifdef MQTT_PUBLISH_FRAMES
void publishFrameQueue(MQTTClient& client, std::queue<Frame>& queue, const char* queue_name) {
    while (!queue.empty()) {
        auto frame = queue.front();

        char topic[100];
        snprintf(topic, 100, "%s/debug/%s/%s", DhwState::getInstance().getDeviceId(), queue_name, frame.getChannelName());

        char hex[MAX_FRAME_SIZE * 2 + 1];
        for (int i=0 ; i<frame.get_buffer_size() ; i++) {
            sprintf(hex + 2*i, "%02X", frame.get_buffer()[i]);
        }
        client.publish(topic, hex);

        queue.pop();
    }
}

void MqttTaskV5::publishFrames() {
    if (xSemaphoreTake(queue_mutex, portMAX_DELAY)) {
        publishFrameQueue(client, frame_queue, "frame");
        publishFrameQueue(client, dropped_queue, "dropped");
        xSemaphoreGive(queue_mutex);
    }
}
#endif // ifdef MQTT_PUBLISH_FRAMES

void MqttTaskV5::subscribe()
{
    for (auto it = command_topics.begin() ; it != command_topics.end() ; it++) {
        LOG.print("subscribe to topic: ");
        LOG.println(it->first);
        client.subscribe(it->first);
    }
}

}  // namespace aquamqtt
