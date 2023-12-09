#include "task/ControllerTask.h"

#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "state/HMIStateProxy.h"

namespace aquamqtt
{

ControllerTask::ControllerTask()
    : mBuffer(false, true, true, "controller")
    , mFlagSeen193(false)
    , mFlagSeen67(false)
    , mLastStatisticsUpdate(0)
    , mTransferBuffer{ 0 }
    , mCRC()
    , mMessagesSent(0){}

void ControllerTask::spawn()
{
    // Avoid using tskIDLE_PRIORITY as that is the lowest possible priority and is generally reserved for the IDLE
    // tasks. app_main() runs in the "main" task on core 0 and has a priority of 1. In most circumstances, you should
    // create tasks of priority 1 or higher.
    TaskHandle_t handle;
    xTaskCreatePinnedToCore(ControllerTask::innerTask, "controllerTaskLoop", 2000, this, 4, &handle, 1);
    esp_task_wdt_add(handle);
}

[[noreturn]] void ControllerTask::innerTask(void* pvParameters)
{
    auto* self = (ControllerTask*) pvParameters;

    self->setup();

    while (true)
    {
        esp_task_wdt_reset();
        self->loop();
    }
}

void ControllerTask::setup() // NOLINT(*-convert-member-functions-to-static)
{
    Serial2.begin(9550, SERIAL_8N2, aquamqtt::config::GPIO_MAIN_RX, aquamqtt::config::GPIO_MAIN_TX);
}

void ControllerTask::loop()
{
    bool printSerialStats = (millis() - mLastStatisticsUpdate) >= 5000;

    vTaskDelay(pdMS_TO_TICKS(50));
    while (Serial2.available())
    {
        int valRead = Serial2.read();

        if (mFlagSeen67 && mFlagSeen193)
        {
            if (valRead == aquamqtt::message::HMI_MESSAGE_IDENTIFIER)
            {
                mFlagSeen67  = false;
                mFlagSeen193 = false;

                if (HMIStateProxy::getInstance().copyFrame(aquamqtt::message::HMI_MESSAGE_IDENTIFIER, mTransferBuffer))
                {
                    uint16_t crc = mCRC.ccitt(mTransferBuffer, aquamqtt::message::HMI_MESSAGE_LENGTH);
                    Serial2.write(mTransferBuffer, aquamqtt::message::HMI_MESSAGE_LENGTH);
                    Serial2.write((uint8_t) (crc >> 8));
                    Serial2.write((uint8_t) (crc & 0xFF));
                    Serial2.flush();
                    mMessagesSent++;
                }
                else
                {
                    Serial.println("[main] no hmi message yet, cannot forward");
                }

                // flush read buffer
                while (Serial2.available())
                {
                    Serial2.read();
                }
                continue;
            }
            else
            {
                mFlagSeen67  = false;
                mFlagSeen193 = false;
            }
        }

        int message = mBuffer.pushByte(valRead);
        if (message == aquamqtt::message::ENERGY_MESSAGE_IDENTIFIER)
        {
            mFlagSeen67  = true;
            mFlagSeen193 = false;
        }
        else if (message == aquamqtt::message::MAIN_MESSAGE_IDENTIFIER)
        {
            mFlagSeen193 = true;
        }
    }

    DHWState::getInstance().updateFrameBufferStatistics(
            2,
            BufferStatistics{ mBuffer.getHandledCount(),
                              mBuffer.getUnhandledCount(),
                              mBuffer.getCRCFailedCount(),
                              mBuffer.getDroppedCount(),
                              mMessagesSent });

    if (printSerialStats)
    {
        Serial.print("[main]: handled=");
        Serial.print(mBuffer.getHandledCount());
        Serial.print(", unhandled=");
        Serial.print(mBuffer.getUnhandledCount());
        Serial.print(", sent=");
        Serial.print(mMessagesSent);
        Serial.print(", crcFails=");
        Serial.print(mBuffer.getCRCFailedCount());
        Serial.print(", dropped bytes=");
        Serial.println(mBuffer.getDroppedCount());

        Serial.print("[main] stack size (words)");
        Serial.println(uxTaskGetStackHighWaterMark(nullptr));

        mLastStatisticsUpdate = millis();
    }
}

}  // namespace aquamqtt
