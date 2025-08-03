#include "task/ListenerTask.h"

#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "state/DHWState.h"

namespace aquamqtt
{

ListenerTask::ListenerTask() : mBuffer(message::FrameBufferChannel::CH_LISTENER), mLastStatisticsUpdate(0){}

void ListenerTask::spawn()
{
    TaskHandle_t handle;
    xTaskCreatePinnedToCore(ListenerTask::innerTask, "listenerTaskLoop", 2000, this, 4, &handle, 1);
    esp_task_wdt_add(handle);
}

[[noreturn]] void ListenerTask::innerTask(void* pvParameters)
{
    auto* self = (ListenerTask*) pvParameters;

    self->setup();

    while (true)
    {
        esp_task_wdt_reset();
        self->loop();
    }
}

void ListenerTask::setup()  // NOLINT(*-convert-member-functions-to-static)
{
    // if passthrough jumper is set, it doesn't make a difference if we read from Serial1 or Serial2
    Serial2.begin(config::DEFAULT_SERIAL_BAUD, config::DEFAULT_SERIAL_CONFIGURATION, config::GPIO_MAIN_RX, config::GPIO_MAIN_TX);
}

void ListenerTask::loop()
{
    bool printSerialStats = (millis() - mLastStatisticsUpdate) >= 5000;

    vTaskDelay(pdMS_TO_TICKS(50));
    while (Serial2.available())
    {
        mBuffer.pushByte(Serial2.read());
    }

    if (printSerialStats)
    {
        DHWState::getInstance().updateFrameBufferStatistics(
                message::FrameBufferChannel::CH_LISTENER,
                BufferStatistics{ mBuffer.getHandledCount(),
                                  mBuffer.getUnhandledCount(),
                                  mBuffer.getCRCFailedCount(),
                                  mBuffer.getDroppedCount(),
                                  0 });

        Serial.print("[listener]: handled=");
        Serial.print(mBuffer.getHandledCount());
        Serial.print(", unhandled=");
        Serial.print(mBuffer.getUnhandledCount());
        Serial.print(", crcFails=");
        Serial.print(mBuffer.getCRCFailedCount());
        Serial.print(", dropped bytes=");
        Serial.println(mBuffer.getDroppedCount());

        mLastStatisticsUpdate = millis();
    }
}

}  // namespace aquamqtt
