#include "task/HMITask.h"

#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "state/HMIStateProxy.h"

namespace aquamqtt
{

HMITask::HMITask()
    : mBuffer(true, false, false, "hmi")
    , mLastStatisticsUpdate(0)
    , mTransferBuffer{ 0 }
    , mCRC()
    , mMessagesSent(0)
{
}

void HMITask::spawn()
{
    // Avoid using tskIDLE_PRIORITY as that is the lowest possible priority and is generally reserved for the IDLE
    // tasks. app_main() runs in the "main" task on core 0 and has a priority of 1. In most circumstances, you should
    // create tasks of priority 1 or higher.
    TaskHandle_t handle;
    xTaskCreatePinnedToCore(HMITask::innerTask, "hmiTaskLoop", 2000, this, 4, &handle, 1);
    esp_task_wdt_add(handle);
}

[[noreturn]] void HMITask::innerTask(void* pvParameters)
{
    auto* self = (HMITask*) pvParameters;

    self->setup();
    while (true)
    {
        esp_task_wdt_reset();
        self->loop();
    }
}

void HMITask::setup()
{
    Serial1.begin(9550, SERIAL_8N2, aquamqtt::config::GPIO_HMI_RX, aquamqtt::config::GPIO_HMI_TX);
}

void HMITask::loop()
{
    bool printSerialStats = (millis() - mLastStatisticsUpdate) >= 5000;

    // request 194 from hmi
    Serial1.write(194);
    Serial1.flush();
    vTaskDelay(pdMS_TO_TICKS(60));

    while (Serial1.available())
    {
        mBuffer.pushByte(Serial1.read());
    }

    // as soon as we have a valid message from the main controller, forward it
    if (DHWState::getInstance().copyFrame(67, mTransferBuffer))
    {
        uint16_t crc = mCRC.ccitt(mTransferBuffer, 31);
        Serial1.write(67);
        Serial1.write(mTransferBuffer, 31);
        Serial1.write((uint8_t) (crc >> 8));
        Serial1.write((uint8_t) (crc & 0xFF));
        Serial1.flush();
        mMessagesSent++;
    }
    else
    {
        Serial.println("[hmi] no energy message yet, cannot forward");
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    // as soon as we have a valid message from the main controller, forward it
    if (DHWState::getInstance().copyFrame(193, mTransferBuffer))
    {
        uint16_t crc = mCRC.ccitt(mTransferBuffer, 37);
        Serial1.write(193);
        Serial1.write(mTransferBuffer, 37);
        Serial1.write((uint8_t) (crc >> 8));
        Serial1.write((uint8_t) (crc & 0xFF));
        Serial1.flush();
        mMessagesSent++;
    }
    else
    {
        Serial.println("[hmi] no main message yet, cannot forward");
    }

    // flush read buffer
    while (Serial1.available())
    {
        Serial1.read();
    }

    vTaskDelay(pdMS_TO_TICKS(200));

    DHWState::getInstance().updateFrameBufferStatistics(
            1,
            BufferStatistics{ mBuffer.getHandledCount(),
                              mBuffer.getUnhandledCount(),
                              mBuffer.getCRCFailedCount(),
                              mBuffer.getDroppedCount(),
                              mMessagesSent });

    if (printSerialStats)
    {

        Serial.print("[hmi]: handled=");
        Serial.print(mBuffer.getHandledCount());
        Serial.print(", unhandled=");
        Serial.print(mBuffer.getUnhandledCount());
        Serial.print(", sent=");
        Serial.print(mMessagesSent);
        Serial.print(", crcFails=");
        Serial.print(mBuffer.getCRCFailedCount());
        Serial.print(", dropped bytes=");
        Serial.println(mBuffer.getDroppedCount());


        Serial.print("[hmi]: stack size (words)");
        Serial.println(uxTaskGetStackHighWaterMark( NULL ));
        Serial.print("[general]: minimum ever heep ");
        Serial.println(xPortGetMinimumEverFreeHeapSize());

        mLastStatisticsUpdate = millis();
    }
}

}  // namespace aquamqtt
