#include "state/DHWState.h"

#include "message/MessageConstants.h"

using namespace aquamqtt::message;

namespace aquamqtt
{
DHWState& DHWState::getInstance()
{
    static DHWState instance;
    return instance;
}

DHWState::DHWState()
    : mNotify(nullptr)
    , mMutex(xSemaphoreCreateMutex())
    , mProtocolVersion(PROTOCOL_UNKNOWN)
    , mChecksumType(CHECKSUM_TYPE_UNKNOWN)
    , mHasHmiMessage(false)
    , mHasMainMessage(false)
    , mHasEnergyMessage(false)
    , mHasErrorMessage(false)
    , mHasExtraMessage(false)
    , mMessageHmi{}
    , mMessageMain{}
    , mMessageEnergy{}
    , mMessageError{}
    , mMessageExtra{}
    , mHmiStats{ 0, 0, 0, 0, 0 }
    , mMainStats{ 0, 0, 0, 0, 0 }
    , mListenerStats{ 0, 0, 0, 0, 0 }
{
}

void DHWState::setListener(const TaskHandle_t handle)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mNotify = handle;

    xSemaphoreGive(mMutex);
}

void DHWState::storeFrame(const uint8_t frameId, const uint8_t payloadLength, uint8_t* payload)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }
    if (frameId == HMI_MESSAGE_IDENTIFIER && memcmp(mMessageHmi, payload, payloadLength) != 0)
    {
        memcpy(mMessageHmi, payload, payloadLength);
        mHasHmiMessage = true;

        if (mNotify != nullptr)
        {
            xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
        }
    }
    else if (frameId == MAIN_MESSAGE_IDENTIFIER && memcmp(mMessageMain, payload, payloadLength) != 0)
    {
        memcpy(mMessageMain, payload, payloadLength);
        mHasMainMessage = true;

        if (mNotify != nullptr)
        {
            xTaskNotifyIndexed(mNotify, 0, (1UL << 7UL), eSetBits);
        }
    }
    else if (frameId == ENERGY_MESSAGE_IDENTIFIER && memcmp(mMessageEnergy, payload, payloadLength) != 0)
    {
        memcpy(mMessageEnergy, payload, payloadLength);
        mHasEnergyMessage = true;

        if (mNotify != nullptr)
        {
            xTaskNotifyIndexed(mNotify, 0, (1UL << 6UL), eSetBits);
        }
    }
    else if (frameId == ERROR_MESSAGE_IDENTIFIER && memcmp(mMessageError, payload, payloadLength) != 0)
    {
        memcpy(mMessageError, payload, payloadLength);
        mHasErrorMessage = true;

        if (mNotify != nullptr)
        {
            xTaskNotifyIndexed(mNotify, 0, (1UL << 5UL), eSetBits);
        }
    }
    else if (frameId == EXTRA_MESSAGE_IDENTIFIER && memcmp(mMessageExtra, payload, payloadLength) != 0)
    {
        memcpy(mMessageExtra, payload, payloadLength);
        mHasExtraMessage = true;

        if (mNotify != nullptr)
        {
            xTaskNotifyIndexed(mNotify, 0, (1UL << 4UL), eSetBits);
        }
    }

    xSemaphoreGive(mMutex);
}

void DHWState::updateFrameBufferStatistics(const FrameBufferChannel source, BufferStatistics statistics)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    if (source == FrameBufferChannel::CH_LISTENER)
    {
        mListenerStats = statistics;
    }
    else if (source == FrameBufferChannel::CH_HMI)
    {
        mHmiStats = statistics;
    }
    else if (source == FrameBufferChannel::CH_MAIN)
    {
        mMainStats = statistics;
    }

    xSemaphoreGive(mMutex);
}

BufferStatistics DHWState::getFrameBufferStatistics(const FrameBufferChannel source)
{
    BufferStatistics statistics{};
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return BufferStatistics{};
    }

    if (source == FrameBufferChannel::CH_LISTENER)
    {
        statistics = mListenerStats;
    }
    else if (source == FrameBufferChannel::CH_HMI)
    {
        statistics = mHmiStats;
    }
    else if (source == FrameBufferChannel::CH_MAIN)
    {
        statistics = mMainStats;
    }

    xSemaphoreGive(mMutex);

    return statistics;
}

size_t DHWState::copyFrame(const uint8_t frameId, uint8_t* buffer, ProtocolVersion& version, ProtocolChecksum& type)
{
    size_t length = 0;

    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return length;
    }

    if (mProtocolVersion != PROTOCOL_UNKNOWN)
    {
        if (frameId == HMI_MESSAGE_IDENTIFIER && mHasHmiMessage)
        {
            length = lengthByFrameIdAndProtocol(frameId, mProtocolVersion);
            memcpy(buffer, mMessageHmi, length);
        }
        else if (frameId == MAIN_MESSAGE_IDENTIFIER && mHasMainMessage)
        {
            length = lengthByFrameIdAndProtocol(frameId, mProtocolVersion);
            memcpy(buffer, mMessageMain, length);
        }
        else if (frameId == ENERGY_MESSAGE_IDENTIFIER && mHasEnergyMessage)
        {
            length = lengthByFrameIdAndProtocol(frameId, mProtocolVersion);
            memcpy(buffer, mMessageEnergy, length);
        }
        else if (frameId == ERROR_MESSAGE_IDENTIFIER && mHasErrorMessage)
        {
            length = lengthByFrameIdAndProtocol(frameId, mProtocolVersion);
            memcpy(buffer, mMessageError, length);
        }
        else if (frameId == EXTRA_MESSAGE_IDENTIFIER && mHasExtraMessage)
        {
            length = lengthByFrameIdAndProtocol(frameId, mProtocolVersion);
            memcpy(buffer, mMessageExtra, length);
        }
    }

    version = mProtocolVersion;
    type    = mChecksumType;

    xSemaphoreGive(mMutex);

    return length;
}

ProtocolVersion DHWState::getVersion() const
{
    ProtocolVersion version = PROTOCOL_UNKNOWN;
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return version;
    }

    version = mProtocolVersion;

    xSemaphoreGive(mMutex);

    return version;
}

void DHWState::setVersion(const ProtocolVersion version)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mProtocolVersion = version;

    xSemaphoreGive(mMutex);
}

void DHWState::setChecksumType(const ProtocolChecksum checksum)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mChecksumType = checksum;

    xSemaphoreGive(mMutex);
}

void DHWState::debugSaveFrameTiming(const uint8_t fromFrameId, const uint8_t toFrameId, const unsigned long millis)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mFrameTiming[fromFrameId][toFrameId] = millis;

    xSemaphoreGive(mMutex);
}

std::map<uint8_t, std::map<uint8_t, unsigned long>> DHWState::debugGetFrameTiming() const
{
    std::map<uint8_t, std::map<uint8_t, unsigned long>> timing;
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return timing;
    }

    timing = mFrameTiming;

    xSemaphoreGive(mMutex);

    return timing;
}

void DHWState::debugAddToDropBuffer(const FrameBufferChannel source, const int val)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    RingBuf<int, HEATPUMP_MAX_FRAME_LENGTH>* buf;
    if (source == FrameBufferChannel::CH_LISTENER)
    {
        buf = &mDroppedBufferListener;
    }
    else if (source == FrameBufferChannel::CH_HMI)
    {
        buf = &mDroppedBufferHmi;
    }
    else
    {
        buf = &mDroppedBufferController;
    }

    if (buf->isFull())
    {
        buf->clear();
    }
    buf->push(val);

    xSemaphoreGive(mMutex);
}

size_t DHWState::debugTakeDropBuffer(const FrameBufferChannel source, uint8_t* buffer)
{

    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return 0;
    }

    size_t cursor = 0;

    RingBuf<int, HEATPUMP_MAX_FRAME_LENGTH>* buf;
    if (source == FrameBufferChannel::CH_LISTENER)
    {
        buf = &mDroppedBufferListener;
    }
    else if (source == FrameBufferChannel::CH_HMI)
    {
        buf = &mDroppedBufferHmi;
    }
    else
    {
        buf = &mDroppedBufferController;
    }

    while (!buf->isEmpty())
    {
        int retVal;
        buf->pop(retVal);
        buffer[cursor] = retVal;
        cursor++;
    }

    xSemaphoreGive(mMutex);

    return cursor;
}

}  // namespace aquamqtt