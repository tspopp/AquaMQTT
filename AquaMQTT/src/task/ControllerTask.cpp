#include "task/ControllerTask.h"

#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "message/MessageConstants.h"
#include "state/HMIStateProxy.h"

namespace aquamqtt
{
ControllerTask::ControllerTask()
    : mBuffer(message::FrameBufferChannel::CH_MAIN)
    , mLastStatisticsUpdate(0)
    , mTransferBuffer{}
    , mMessagesSent(0)
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
    Serial2.begin(9550, SERIAL_8N2, config::GPIO_MAIN_RX, config::GPIO_MAIN_TX);
}

void ControllerTask::loop()
{
    const bool printSerialStats = (millis() - mLastStatisticsUpdate) >= 5000;

    vTaskDelay(pdMS_TO_TICKS(5));
    while (Serial2.available())
    {
        const int valRead = Serial2.read();

        if (valRead == 194) {
            // legacy/next sequence  67 -> 193 -> (74) -> 194
            // operator[] is safe to call even it is not properly filled, yet
            if (mSequence[0] == 67 && mSequence[1] == 193 || mSequence[0] == 193 && mSequence[1] == 74) {
                sendMessage194();
                flushReadBuffer();
                mSequence.clear();
                continue;
            }

            // odyssee sequence 67 -> 217 -> 193 -> 194 -> 74
            if (mSequence[0] == 217 && mSequence[1] == 193) {
                sendMessage194();
                flushReadBuffer();
                mSequence.clear();
                continue;
            }

        }

        if (const int result = mBuffer.pushByte(valRead); result > 0) {
            mSequence.pushOverwrite(result);
        }

    }

    if (printSerialStats)
    {
        DHWState::getInstance().updateFrameBufferStatistics(
                message::FrameBufferChannel::CH_MAIN,
                BufferStatistics{ mBuffer.getHandledCount(),
                                  mBuffer.getUnhandledCount(),
                                  mBuffer.getCRCFailedCount(),
                                  mBuffer.getDroppedCount(),
                                  mMessagesSent });

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
    message::ProtocolVersion  version      = message::PROTOCOL_UNKNOWN;
    message::ProtocolChecksum checksumType = message::CHECKSUM_TYPE_UNKNOWN;
    size_t                    length       = HMIStateProxy::getInstance()
                            .copyFrame(message::HMI_MESSAGE_IDENTIFIER, mTransferBuffer, version, checksumType);

    if (length <= 0 || version == message::PROTOCOL_UNKNOWN)
    {
        Serial.println("[main] no hmi message yet, cannot forward");
        return;
    }

    Serial2.write(mTransferBuffer, length);
    if (checksumType == message::CHECKSUM_TYPE_CRC16)
    {
        uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
        Serial2.write((uint8_t) (crc >> 8));
        Serial2.write((uint8_t) (crc & 0xFF));
    }
    else if (checksumType == message::CHECKSUM_TYPE_XOR)
    {
        uint8_t checksum = message::generateXorChecksum(mTransferBuffer, length);
        Serial2.write(checksum);
    }
    Serial2.flush();
    mMessagesSent++;
}
}  // namespace aquamqtt
