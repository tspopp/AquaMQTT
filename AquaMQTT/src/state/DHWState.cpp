#include "state/DHWState.h"

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
    if (frameId == aquamqtt::message::HMI_MESSAGE_IDENTIFIER && payloadLength == aquamqtt::message::HMI_MESSAGE_LENGTH
        && memcmp(mMessageHmi, payload, payloadLength) != 0)
    {
        memcpy(mMessageHmi, payload, payloadLength);
        mHasHmiMessage = true;

        if (mNotify != nullptr)
        {
            xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
        }
    }
    else if (
            frameId == aquamqtt::message::MAIN_MESSAGE_IDENTIFIER
            && payloadLength == aquamqtt::message::MAIN_MESSAGE_LENGTH
            && memcmp(mMessageMain, payload, payloadLength) != 0)
    {
        memcpy(mMessageMain, payload, payloadLength);
        mHasMainMessage = true;

        if (mNotify != nullptr)
        {
            xTaskNotifyIndexed(mNotify, 0, (1UL << 7UL), eSetBits);
        }
    }
    else if (
            frameId == aquamqtt::message::ENERGY_MESSAGE_IDENTIFIER
            && payloadLength == aquamqtt::message::ENERGY_MESSAGE_LENGTH
            && memcmp(mMessageEnergy, payload, payloadLength) != 0)
    {
        memcpy(mMessageEnergy, payload, payloadLength);
        mHasEnergyMessage = true;

        if (mNotify != nullptr)
        {
            xTaskNotifyIndexed(mNotify, 0, (1UL << 6UL), eSetBits);
        }
    }
    else if (
            frameId == aquamqtt::message::ERROR_MESSAGE_IDENTIFIER
            && payloadLength == aquamqtt::message::ERROR_MESSAGE_LENGTH
            && memcmp(mMessageError, payload, payloadLength) != 0)
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
bool DHWState::copyFrame(uint8_t frameId, uint8_t* buffer)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return false;
    }

    bool handled = false;
    if (frameId == aquamqtt::message::HMI_MESSAGE_IDENTIFIER && mHasHmiMessage)
    {
        memcpy(buffer, mMessageHmi, aquamqtt::message::HMI_MESSAGE_LENGTH);
        handled = true;
    }
    else if (frameId == aquamqtt::message::MAIN_MESSAGE_IDENTIFIER && mHasMainMessage)
    {
        memcpy(buffer, mMessageMain, aquamqtt::message::MAIN_MESSAGE_LENGTH);
        handled = true;
    }
    else if (frameId == aquamqtt::message::ENERGY_MESSAGE_IDENTIFIER && mHasEnergyMessage)
    {
        memcpy(buffer, mMessageEnergy, aquamqtt::message::ENERGY_MESSAGE_LENGTH);
        handled = true;
    }
    else if (frameId == aquamqtt::message::ERROR_MESSAGE_IDENTIFIER && mHasErrorMessage)
    {
        memcpy(buffer, mMessageError, aquamqtt::message::ERROR_MESSAGE_LENGTH);
        handled = true;
    }

    xSemaphoreGive(mMutex);

    return handled;
}
}  // namespace aquamqtt