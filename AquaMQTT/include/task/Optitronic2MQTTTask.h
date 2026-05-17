#ifndef AQUAMQTT_OPTITRONIC2_MQTT_TASK_H
#define AQUAMQTT_OPTITRONIC2_MQTT_TASK_H

#include <Arduino.h>
#include <MQTTClient.h>
#include <WiFiClient.h>

namespace aquamqtt
{

/**
 * MQTT task for Optitronic 2 protocol.
 * Publishes register data as human-readable MQTT topics and
 * handles incoming commands (write requests) via MQTT.
 *
 * Also provides HA MQTT auto-discovery.
 */
class Optitronic2MQTTTask
{
public:
    Optitronic2MQTTTask();

    void spawn();

    // Called by static MQTT callback trampoline
    void handleMessage(const String& topic, const String& payload);

    // Called by ModbusListenerTask to check if there are pending writes
    bool hasPendingWrite() const;
    bool dequeuePendingWrite(uint16_t& reg, uint16_t& value);

    // Queue a register write (used by MQTT handler and web API)
    void queueWrite(uint16_t reg, uint16_t value);

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    void setup();
    void loop();

    void connectMqtt();
    void publishAll();
    void publishSensors();
    void publishSettings();
    void publishState();
    void publishInstaller();
    void publishStats();
    void publishDiscovery();

    // MQTT write command: sends Modbus write via serial
    void handleSetDhwSetpoint(float temp);
    void handleSetProgram(uint16_t program);
    void handleSetExtInputFunction(uint16_t fn);
    void handleSetForceHeating(bool enable);
    void handleSetQuickHeat(bool activate, float target);
    void handleTriggerAntiLegionella();

    // Publish helper
    void publishFloat(const char* subtopic, float value, uint8_t decimals = 1);
    void publishInt(const char* subtopic, int value);
    void publishString(const char* subtopic, const char* value);
    void publishBool(const char* subtopic, bool value);

    MQTTClient    mMQTTClient;
    WiFiClient    mWiFiClient;
    TaskHandle_t  mTaskHandle;
    char          mTopicBuffer[128];
    char          mPayloadBuffer[64];

    unsigned long mLastFullUpdate;
    unsigned long mLastStatsUpdate;
    bool          mPublishedDiscovery;

    // Write queue (commands received via MQTT, forwarded to serial in listener task context)
    struct WriteCommand
    {
        uint16_t reg;
        uint16_t value;
        bool     pending;
    };
    static constexpr uint8_t WRITE_QUEUE_SIZE = 8;
    WriteCommand mWriteQueue[WRITE_QUEUE_SIZE];
    uint8_t      mWriteQueueHead;
    SemaphoreHandle_t mWriteMutex;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_OPTITRONIC2_MQTT_TASK_H
