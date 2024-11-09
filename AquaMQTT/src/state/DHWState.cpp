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
    : mMutex(xSemaphoreCreateMutex())
    , mNotify(nullptr)
    , mHasEnergyMessage(false)
    , mHasMainMessage(false)
    , mHasHmiMessage(false)
    , mHasErrorMessage(false)
    , mMessageHmi{ 0 }
    , mMessageMain{ 0 }
    , mMessageEnergy{ 0 }
    , mMessageError{ 0 }
    , mHmiStats{ 0, 0, 0, 0, 0 }
    , mMainStats{ 0, 0, 0, 0, 0 }
    , mListenerStats{ 0, 0, 0, 0, 0 }
    , mProtocolVersion(PROTOCOL_UNKNOWN)
{
}
void DHWState::setListener(TaskHandle_t handle)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mNotify = handle;

    xSemaphoreGive(mMutex);
}
void DHWState::storeFrame(uint8_t frameId, uint8_t payloadLength, uint8_t* payload)
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

    xSemaphoreGive(mMutex);
}
void DHWState::updateFrameBufferStatistics(uint8_t source, BufferStatistics statistics)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    if (source == 0)
    {
        mListenerStats = statistics;
    }
    else if (source == 1)
    {
        mHmiStats = statistics;
    }
    else if (source == 2)
    {
        mMainStats = statistics;
    }

    xSemaphoreGive(mMutex);
}
BufferStatistics DHWState::getFrameBufferStatistics(uint8_t source)
{
    BufferStatistics statistics{};
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return BufferStatistics{};
    }

    if (source == 0)
    {
        statistics = mListenerStats;
    }
    else if (source == 1)
    {
        statistics = mHmiStats;
    }
    else if (source == 2)
    {
        statistics = mMainStats;
    }

    xSemaphoreGive(mMutex);

    return statistics;
}
size_t DHWState::copyFrame(uint8_t frameId, uint8_t* buffer, message::ProtocolVersion& version)
{
    size_t length = 0;

    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return length;
    }

    if (frameId == HMI_MESSAGE_IDENTIFIER && mHasHmiMessage)
    {
        length = mProtocolVersion == PROTOCOL_LEGACY ? HMI_MESSAGE_LENGTH_LEGACY : HMI_MESSAGE_LENGTH_NEXT;
        memcpy(buffer, mMessageHmi, length);
    }
    else if (frameId == MAIN_MESSAGE_IDENTIFIER && mHasMainMessage)
    {
        length = mProtocolVersion == PROTOCOL_LEGACY ? MAIN_MESSAGE_LENGTH_LEGACY : MAIN_MESSAGE_LENGTH_NEXT;
        memcpy(buffer, mMessageMain, length);
    }
    else if (frameId == ENERGY_MESSAGE_IDENTIFIER && mHasEnergyMessage)
    {
        length = mProtocolVersion == PROTOCOL_LEGACY ? ENERGY_MESSAGE_LENGTH_LEGACY : ENERGY_MESSAGE_LENGTH_NEXT;
        memcpy(buffer, mMessageEnergy, length);
    }
    else if (frameId == ERROR_MESSAGE_IDENTIFIER && mHasErrorMessage)
    {
        length = mProtocolVersion == PROTOCOL_LEGACY ? ERROR_MESSAGE_LENGTH_LEGACY : ERROR_MESSAGE_LENGTH_NEXT;
        memcpy(buffer, mMessageError, length);
    }

    version = mProtocolVersion;

    xSemaphoreGive(mMutex);

    return length;
}
message::ProtocolVersion DHWState::getVersion()
{
    message::ProtocolVersion version = message::PROTOCOL_UNKNOWN;
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return version;
    }

    version = mProtocolVersion;

    xSemaphoreGive(mMutex);

    return version;
}

void DHWState::setVersion(message::ProtocolVersion version)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }

    mProtocolVersion = version;

    xSemaphoreGive(mMutex);
}
}  // namespace aquamqtt