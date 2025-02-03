#include "task/HMITask.h"

#include <esp_task_wdt.h>
#include <message/Factory.h>

#include "config/Configuration.h"
#include "message/MessageConstants.h"
#include "state/MainStateProxy.h"

namespace aquamqtt
{

HMITask::HMITask()
    : mBuffer(message::FrameBufferChannel::CH_HMI)
    , mLastStatisticsUpdate(0)
    , mLastMessageSent(0)
    , mLastEmittedRequestId(UINT8_MAX)
    , mTransferBuffer{}
    , mMessagesSent(0)
    , mState(HMITaskState::REQUEST_194)
    , mVersion(message::PROTOCOL_UNKNOWN)
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
    Serial1.begin(9550, SERIAL_8N2, config::GPIO_HMI_RX, config::GPIO_HMI_TX);
    pinMode(config::GPIO_ENABLE_TX_HMI, OUTPUT);
}

void HMITask::loop()
{
    if (mVersion == message::ProtocolVersion::PROTOCOL_UNKNOWN)
    {
        mVersion = DHWState::getInstance().getVersion();
    }

    const bool printSerialStats   = (millis() - mLastStatisticsUpdate) >= 5000;
    const bool performStateChange = (millis() - mLastMessageSent) >= message::MESSAGE_PERIOD_MS;

    while (Serial1.available())
    {
        mBuffer.pushByte(Serial1.read());
    }

    switch (mState)
    {
        case HMITaskState::REQUEST_194:
            mState           = HMITaskState::SLEEP_194;
            mLastMessageSent = millis();
            digitalWrite(config::GPIO_ENABLE_TX_HMI, HIGH);
            Serial1.write(message::HMI_MESSAGE_IDENTIFIER);
            Serial1.flush();
            digitalWrite(config::GPIO_ENABLE_TX_HMI, LOW);
            break;
        case HMITaskState::SLEEP_194:
            if (mVersion == message::ProtocolVersion::PROTOCOL_ODYSSEE)
            {
                awaitStateChangeTo(HMITaskState::SEND_74POST, performStateChange);
            }
            else
            {
                awaitStateChangeTo(HMITaskState::SEND_67, performStateChange);
            }
            break;
        case HMITaskState::SEND_67:
            mState           = HMITaskState::SLEEP_67;
            mLastMessageSent = millis();
            sendMessage67();
            flushReadBuffer();
            break;
        case HMITaskState::SLEEP_67:
            if (mVersion == message::ProtocolVersion::PROTOCOL_ODYSSEE)
            {
                awaitStateChangeTo(HMITaskState::SEND_217, performStateChange);
            }
            else
            {
                awaitStateChangeTo(HMITaskState::SEND_193, performStateChange);
            }
            break;
        case HMITaskState::SEND_74POST:
            mState           = HMITaskState::SLEEP_74POST;
            mLastMessageSent = millis();
            sendMessage74();
            flushReadBuffer();
            break;
        case HMITaskState::SLEEP_74POST:
            awaitStateChangeTo(HMITaskState::SEND_67, performStateChange);
            break;
        case HMITaskState::SEND_193:
            mState           = HMITaskState::SLEEP_193;
            mLastMessageSent = millis();
            sendMessage193();
            flushReadBuffer();
            break;
        case HMITaskState::SLEEP_193:
            if (mVersion == message::ProtocolVersion::PROTOCOL_ODYSSEE)
            {
                awaitStateChangeTo(HMITaskState::REQUEST_194, performStateChange);
            }
            else
            {
                awaitStateChangeTo(HMITaskState::SEND_74PRE, performStateChange);
            }
            break;
        case HMITaskState::SEND_217:
            mState           = HMITaskState::SLEEP_217;
            mLastMessageSent = millis();
            sendMessage217();
            flushReadBuffer();
            break;
        case HMITaskState::SLEEP_217:
            awaitStateChangeTo(HMITaskState::SEND_193, performStateChange);
            break;
        case HMITaskState::SEND_74PRE:
            mState           = HMITaskState::SLEEP_74PRE;
            mLastMessageSent = millis();
            sendMessage74();
            flushReadBuffer();
            break;
        case HMITaskState::SLEEP_74PRE:
            awaitStateChangeTo(HMITaskState::REQUEST_194, performStateChange);
            break;
    }

    if (printSerialStats)
    {

        DHWState::getInstance().updateFrameBufferStatistics(
                message::FrameBufferChannel::CH_HMI,
                BufferStatistics{ mBuffer.getHandledCount(),
                                  mBuffer.getUnhandledCount(),
                                  mBuffer.getCRCFailedCount(),
                                  mBuffer.getDroppedCount(),
                                  mMessagesSent });

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

void HMITask::awaitStateChangeTo(const HMITaskState state, const bool performStateChange)
{
    if (performStateChange)
    {
        mState = state;
    }
    else
    {
        vTaskDelay(pdMS_TO_TICKS(5));
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
    message::ProtocolVersion  version;
    message::ProtocolChecksum checksumType = message::CHECKSUM_TYPE_UNKNOWN;
    const size_t              length       = MainStateProxy::getInstance()
                                  .copyFrame(message::MAIN_MESSAGE_IDENTIFIER, mTransferBuffer, version, checksumType);

    if (length <= 0 || version == message::PROTOCOL_UNKNOWN)
    {
        Serial.println("[hmi] no main message yet, cannot forward");
        return;
    }

    digitalWrite(config::GPIO_ENABLE_TX_HMI, HIGH);
    Serial1.write(message::MAIN_MESSAGE_IDENTIFIER);
    Serial1.write(mTransferBuffer, length);

    if (checksumType == message::CHECKSUM_TYPE_CRC16)
    {
        const uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
        Serial1.write((uint8_t) (crc >> 8));
        Serial1.write((uint8_t) (crc & 0xFF));
    }
    else
    {
        const uint8_t checksum = message::generateXorChecksum(mTransferBuffer, length);
        Serial1.write(checksum);
    }

    Serial1.flush();
    digitalWrite(config::GPIO_ENABLE_TX_HMI, LOW);
    mMessagesSent++;
}

void HMITask::sendMessage67()
{
    message::ProtocolVersion  version;
    message::ProtocolChecksum checksumType = message::CHECKSUM_TYPE_UNKNOWN;

    const size_t length = MainStateProxy::getInstance().copyFrame(
            message::ENERGY_MESSAGE_IDENTIFIER,
            mTransferBuffer,
            version,
            checksumType);

    if (length <= 0 || version == message::PROTOCOL_UNKNOWN)
    {
        Serial.println("[hmi] no energy message yet, cannot forward");
        return;
    }

    digitalWrite(config::GPIO_ENABLE_TX_HMI, HIGH);
    Serial1.write(message::ENERGY_MESSAGE_IDENTIFIER);
    Serial1.write(mTransferBuffer, length);

    if (checksumType == message::CHECKSUM_TYPE_CRC16)
    {
        const uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
        Serial1.write((uint8_t) (crc >> 8));
        Serial1.write((uint8_t) (crc & 0xFF));
    }
    else
    {
        const uint8_t checksum = message::generateXorChecksum(mTransferBuffer, length);
        Serial1.write(checksum);
    }
    Serial1.flush();
    digitalWrite(config::GPIO_ENABLE_TX_HMI, LOW);
    mMessagesSent++;
}

void HMITask::sendMessage74()
{
    // check if the HMI is requesting an error message
    uint8_t requestId = UINT8_MAX;
    {
        message::ProtocolVersion  version;
        message::ProtocolChecksum checksumType = message::CHECKSUM_TYPE_UNKNOWN;

        const size_t length = MainStateProxy::getInstance().copyFrame(
                message::HMI_MESSAGE_IDENTIFIER,
                mTransferBuffer,
                version,
                checksumType);

        if (length > 0)
        {
            const auto message = createHmiMessageFromBuffer(version, mTransferBuffer);
            requestId          = message->getAttr(message::HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED);
        }
    }

    // Check if we already emitted that error message, because we emit error messages just once
    if (mLastEmittedRequestId == requestId)
    {
        // already emitted, no change
        return;
    }

    // check if we have the requested error message in cache
    uint8_t                   availableRequestId = 0;
    message::ProtocolVersion  version;
    message::ProtocolChecksum checksumType = message::CHECKSUM_TYPE_UNKNOWN;
    const size_t              length       = MainStateProxy::getInstance().copyFrame(
            message::ERROR_MESSAGE_IDENTIFIER,
            mTransferBuffer,
            version,
            checksumType);
    if (length > 0)
    {
        const auto errorMessage = message::createErrorMessageFromBuffer(version, mTransferBuffer);
        availableRequestId      = errorMessage->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID);
    }

    // emit the error message
    if (requestId == availableRequestId)
    {
        digitalWrite(config::GPIO_ENABLE_TX_HMI, HIGH);
        Serial1.write(message::ERROR_MESSAGE_IDENTIFIER);
        Serial1.write(mTransferBuffer, length);

        if (checksumType == message::CHECKSUM_TYPE_CRC16)
        {
            const uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
            Serial1.write((uint8_t) (crc >> 8));
            Serial1.write((uint8_t) (crc & 0xFF));
        }
        else
        {
            const uint8_t checksum = message::generateXorChecksum(mTransferBuffer, length);
            Serial1.write(checksum);
        }
        Serial1.flush();
        digitalWrite(config::GPIO_ENABLE_TX_HMI, LOW);
        mMessagesSent++;
        mLastEmittedRequestId = requestId;
    }
}

void HMITask::sendMessage217()
{
    message::ProtocolVersion  version;
    message::ProtocolChecksum checksumType = message::CHECKSUM_TYPE_UNKNOWN;

    const size_t length = MainStateProxy::getInstance().copyFrame(
            message::EXTRA_MESSAGE_IDENTIFIER,
            mTransferBuffer,
            version,
            checksumType);

    if (length <= 0 || version == message::PROTOCOL_UNKNOWN)
    {
        Serial.println("[hmi] no power message yet, cannot forward");
        return;
    }

    digitalWrite(config::GPIO_ENABLE_TX_HMI, HIGH);
    Serial1.write(message::EXTRA_MESSAGE_IDENTIFIER);
    Serial1.write(mTransferBuffer, length);

    if (checksumType == message::CHECKSUM_TYPE_CRC16)
    {
        const uint16_t crc = mCRC.ccitt(mTransferBuffer, length);
        Serial1.write((uint8_t) (crc >> 8));
        Serial1.write((uint8_t) (crc & 0xFF));
    }
    else
    {
        const uint8_t checksum = message::generateXorChecksum(mTransferBuffer, length);
        Serial1.write(checksum);
    }
    Serial1.flush();
    digitalWrite(config::GPIO_ENABLE_TX_HMI, LOW);
    mMessagesSent++;
}

}  // namespace aquamqtt
