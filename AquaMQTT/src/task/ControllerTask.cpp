#include "task/ControllerTask.h"

#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "state/HMIStateProxy.h"

namespace aquamqtt
{

ControllerTask::ControllerTask()
    : mBuffer(false, true, true, true)
    , mLastStatisticsUpdate(0)
    , mTransferBuffer{ 0 }
    , mCRC()
    , mMessagesSent(0)
    , mState(ControllerTaskState::AWAITING_67)
{
}

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

void ControllerTask::setup()  // NOLINT(*-convert-member-functions-to-static)
{
    Serial2.begin(9550, SERIAL_8N2, aquamqtt::config::GPIO_MAIN_RX, aquamqtt::config::GPIO_MAIN_TX);
}

void ControllerTask::loop()
{
    bool printSerialStats = (millis() - mLastStatisticsUpdate) >= 5000;

    vTaskDelay(pdMS_TO_TICKS(5));
    while (Serial2.available())
    {
        int valRead = Serial2.read();

        switch (mState)
        {
            case ControllerTaskState::AWAITING_67:
                if (mBuffer.pushByte(valRead) == aquamqtt::message::ENERGY_MESSAGE_IDENTIFIER)
                {
                    mState = ControllerTaskState::AWAITING_193;
                }
                break;
            case ControllerTaskState::AWAITING_193:
            {
                int processedMessageId = mBuffer.pushByte(valRead);
                if (processedMessageId == aquamqtt::message::MAIN_MESSAGE_IDENTIFIER)
                {
                    mState = ControllerTaskState::CHECK_FOR_HMI_TRIGGER;
                }
                else if (processedMessageId != 0)
                {
                    mState = ControllerTaskState::AWAITING_67;
                }
                break;
            }
            case ControllerTaskState::CHECK_FOR_HMI_TRIGGER:
                if (valRead == aquamqtt::message::ERROR_MESSAGE_IDENTIFIER)
                {
                    mBuffer.pushByte(valRead);
                    mState = ControllerTaskState::AWAITING_74;
                    continue;
                }

                if (valRead == aquamqtt::message::HMI_MESSAGE_IDENTIFIER)
                {
                    sendMessage194();
                    flushReadBuffer();
                }

                mState = ControllerTaskState::AWAITING_67;
                break;
            case ControllerTaskState::AWAITING_74:
            {
                int processedMessageId = mBuffer.pushByte(valRead);
                if (processedMessageId == aquamqtt::message::ERROR_MESSAGE_IDENTIFIER)
                {
                    mState = ControllerTaskState::CHECK_FOR_HMI_TRIGGER;
                }
                else if (processedMessageId != 0)
                {
                    mState = ControllerTaskState::AWAITING_67;
                }
                break;
            }
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
void ControllerTask::flushReadBuffer()
{
    while (Serial2.available())
    {
        Serial2.read();
    }
}
void ControllerTask::sendMessage194()
{
    message::ProtocolVersion version = message::PROTOCOL_UNKNOWN;
    size_t length = HMIStateProxy::getInstance().copyFrame(message::HMI_MESSAGE_IDENTIFIER, mTransferBuffer, version);
    if (version == message::PROTOCOL_LEGACY)
    {
        uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
        Serial2.write(mTransferBuffer, length);
        Serial2.write((uint8_t) (crc >> 8));
        Serial2.write((uint8_t) (crc & 0xFF));
        Serial2.flush();
        mMessagesSent++;
    }
    else if(version == message::PROTOCOL_NEXT){
        // TODO
        Serial.println("[main] mitm is not yet implemented for protocol next");
    }
    else
    {
        Serial.println("[main] no hmi message yet, cannot forward");
    }
}

}  // namespace aquamqtt
