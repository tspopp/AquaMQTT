#ifndef AQUAMQTT_DHWSTATE_H
#define AQUAMQTT_DHWSTATE_H

#include <Arduino.h>

#include "message/MessageConstants.h"

namespace aquamqtt
{

struct BufferStatistics
{
    uint64_t mshHandled;
    uint64_t msgUnhandled;
    uint64_t msgCRCFail;
    uint64_t droppedBytes;
    uint64_t msgSent;
};

/**
 * DHWState contains the original state originated by the original DHW controllers
 * FrameBuffers are writing complete frames to the DHWState storing the last known messages
 * In case a message has changed, it will trigger the mqtt task to publish updates to the broker.
 */
class DHWState
{
public:
    static DHWState& getInstance()
    {
        static DHWState instance;
        return instance;
    }

    virtual ~DHWState() = default;

    DHWState(const DHWState&) = delete;

private:
    DHWState()
        : mMutex(xSemaphoreCreateMutex())
        , mNotify(nullptr)
        , mHasEnergyMessage(false)
        , mHasMainMessage(false)
        , mHasHmiMessage(false)
        , mMessageHmi{ 0 }
        , mMessageMain{ 0 }
        , mMessageEnergy{ 0 }
        , mHmiStats{ 0, 0, 0, 0, 0 }
        , mMainStats{ 0, 0, 0, 0, 0 }
        , mListenerStats{ 0, 0, 0, 0, 0 }
    {
    }

    DHWState& operator=(const DHWState&) = delete;

public:
    void setListener(TaskHandle_t handle)
    {
        if (!xSemaphoreTake(mMutex, portMAX_DELAY))
        {
            return;
        }

        mNotify = handle;

        xSemaphoreGive(mMutex);
    }

    void storeFrame(uint8_t frameId, uint8_t payloadLength, uint8_t* payload)
    {
        if (!xSemaphoreTake(mMutex, portMAX_DELAY))
        {
            return;
        }
        if (frameId == aquamqtt::message::HMI_MESSAGE_IDENTIFIER
            && payloadLength == aquamqtt::message::HMI_MESSAGE_LENGTH
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

        xSemaphoreGive(mMutex);
    }

    void updateFrameBufferStatistics(uint8_t source, BufferStatistics statistics)
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

    BufferStatistics getFrameBufferStatistics(uint8_t source)
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

    bool copyFrame(uint8_t frameId, uint8_t* buffer)
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

        xSemaphoreGive(mMutex);

        return handled;
    }

private:
    TaskHandle_t mNotify;

    SemaphoreHandle_t mMutex;

    bool mHasHmiMessage;
    bool mHasMainMessage;
    bool mHasEnergyMessage;

    uint8_t mMessageHmi[aquamqtt::message::HMI_MESSAGE_LENGTH];
    uint8_t mMessageMain[aquamqtt::message::MAIN_MESSAGE_LENGTH];
    uint8_t mMessageEnergy[aquamqtt::message::ENERGY_MESSAGE_LENGTH];

    BufferStatistics mHmiStats;
    BufferStatistics mMainStats;
    BufferStatistics mListenerStats;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_DHWSTATE_H
