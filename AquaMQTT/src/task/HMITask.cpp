#include "task/HMITask.h"

#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "message/IErrorMessage.h"
#include "message/IHMIMessage.h"
#include "message/MessageConstants.h"
#include "message/legacy/ErrorMessage.h"
#include "message/legacy/HMIMessage.h"
#include "state/HMIStateProxy.h"
#include "state/MainStateProxy.h"

namespace aquamqtt
{

HMITask::HMITask()
    : mBuffer(true, false, false, false)
    , mLastEmittedRequestId(UINT8_MAX)
    , mLastStatisticsUpdate(0)
    , mLastMessageSent(0)
    , mTransferBuffer{ 0 }
    , mCRC()
    , mState(HMITaskState::REQUESTING_194)
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

void HMITask::setup()  // NOLINT(*-convert-member-functions-to-static)
{
    Serial1.begin(9550, SERIAL_8N2, aquamqtt::config::GPIO_HMI_RX, aquamqtt::config::GPIO_HMI_TX);
}

void HMITask::loop()
{
    bool printSerialStats   = (millis() - mLastStatisticsUpdate) >= 5000;
    bool performStateChange = (millis() - mLastMessageSent) >= aquamqtt::message::MESSAGE_PERIOD_MS;

    while (Serial1.available())
    {
        mBuffer.pushByte(Serial1.read());
    }

    switch (mState)
    {
        case HMITaskState::REQUESTING_194:
            Serial1.write(aquamqtt::message::HMI_MESSAGE_IDENTIFIER);
            Serial1.flush();
            mState           = HMITaskState::SLEEP_194;
            mLastMessageSent = millis();
            break;
        case HMITaskState::SLEEP_194:
            if (performStateChange)
            {
                mState = HMITaskState::SENDING_67;
            }
            else
            {
                vTaskDelay(pdMS_TO_TICKS(5));
            }
            break;
        case HMITaskState::SENDING_67:
            sendMessage67();
            flushReadBuffer();
            mState           = HMITaskState::SLEEP_67;
            mLastMessageSent = millis();
            break;
        case HMITaskState::SLEEP_67:
            if (performStateChange)
            {
                mState = HMITaskState::SENDING_193;
            }
            else
            {
                vTaskDelay(pdMS_TO_TICKS(5));
            }
            break;
        case HMITaskState::SENDING_193:
            sendMessage193();
            flushReadBuffer();
            mState           = HMITaskState::SLEEP_193;
            mLastMessageSent = millis();
            break;
        case HMITaskState::SLEEP_193:
            if (performStateChange)
            {
                mState = HMITaskState::SENDING_74;
            }
            else
            {
                vTaskDelay(pdMS_TO_TICKS(5));
            }
            break;
        case HMITaskState::SENDING_74:
            sendMessage74();
            flushReadBuffer();
            mState           = HMITaskState::SLEEP_74;
            mLastMessageSent = millis();
            break;
        case HMITaskState::SLEEP_74:
            if (performStateChange)
            {
                mState = HMITaskState::REQUESTING_194;
            }
            else
            {
                vTaskDelay(pdMS_TO_TICKS(5));
            }
            break;
    }

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
        Serial.println(uxTaskGetStackHighWaterMark(nullptr));
        Serial.print("[general]: minimum ever heep ");
        Serial.println(xPortGetMinimumEverFreeHeapSize());

        mLastStatisticsUpdate = millis();
    }
}
void HMITask::flushReadBuffer()
{
    while (Serial1.available())
    {
        Serial1.read();
    }
}
void HMITask::sendMessage193()
{
    message::ProtocolVersion version;
    size_t                   length = MainStateProxy::getInstance().copyFrame(
            message::MAIN_MESSAGE_IDENTIFIER,
            mTransferBuffer,
            version);
    if (length > 0)
    {
        if (version == message::PROTOCOL_LEGACY)
        {
            uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
            Serial1.write(message::MAIN_MESSAGE_IDENTIFIER);
            Serial1.write(mTransferBuffer, length);
            Serial1.write((uint8_t) (crc >> 8));
            Serial1.write((uint8_t) (crc & 0xFF));
            Serial1.flush();
            mMessagesSent++;
        }
        else
        {
            // TOOD: not implemented
        }
    }
    else
    {
        Serial.println("[hmi] no main message yet, cannot forward");
    }
}
void HMITask::sendMessage67()
{
    message::ProtocolVersion version;
    size_t                   length = MainStateProxy::getInstance().copyFrame(
            message::ENERGY_MESSAGE_IDENTIFIER,
            mTransferBuffer,
            version);
    if (length > 0)
    {
        if (version == message::PROTOCOL_LEGACY)
        {
            uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
            Serial1.write(message::ENERGY_MESSAGE_IDENTIFIER);
            Serial1.write(mTransferBuffer, length);
            Serial1.write((uint8_t) (crc >> 8));
            Serial1.write((uint8_t) (crc & 0xFF));
            Serial1.flush();
            mMessagesSent++;
        }
        else
        {
            // TOOD: not implemented
        }
    }
    else
    {
        Serial.println("[hmi] no energy message yet, cannot forward");
    }
}

void HMITask::sendMessage74()
{
    // check if the HMI is requesting an error message
    uint8_t requestId = UINT8_MAX;
    {
        message::ProtocolVersion version;
        size_t                   length = MainStateProxy::getInstance().copyFrame(
                aquamqtt::message::HMI_MESSAGE_IDENTIFIER,
                mTransferBuffer,
                version);
        if (length > 0)
        {
            if (version == message::PROTOCOL_LEGACY)
            {
                aquamqtt::message::legacy::HMIMessage hmiMessage(mTransferBuffer);
                requestId = hmiMessage.errorRequestId();
            }
            else
            {
                // TODO: not implemented
            }
        }
    }

    // Check if we already emitted that error message, because we emit error messages just once
    if (mLastEmittedRequestId == requestId)
    {
        // already emitted, no change
        return;
    }

    // check if we have the requested error message in cache
    uint8_t                  availableRequestId = 0;
    message::ProtocolVersion version;
    size_t                   length = MainStateProxy::getInstance().copyFrame(
            aquamqtt::message::ERROR_MESSAGE_IDENTIFIER,
            mTransferBuffer,
            version);
    if (length > 0)
    {
        if (version == message::PROTOCOL_LEGACY)
        {
            aquamqtt::message::legacy::ErrorMessage errorMessage(mTransferBuffer);
            availableRequestId = errorMessage.errorRequestId();
        }
        else
        {
            // TODO: not implemented
        }
    }

    // emit the error message
    if (requestId == availableRequestId)
    {
        if(version == message::PROTOCOL_LEGACY)
        {
            uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
            Serial1.write(aquamqtt::message::ERROR_MESSAGE_IDENTIFIER);
            Serial1.write(mTransferBuffer, length);
            Serial1.write((uint8_t) (crc >> 8));
            Serial1.write((uint8_t) (crc & 0xFF));
            Serial1.flush();
            mMessagesSent++;
            mLastEmittedRequestId = requestId;
        } else {
            // TODO: not implemented
        }
    }
}

}  // namespace aquamqtt
