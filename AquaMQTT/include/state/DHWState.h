#ifndef AQUAMQTT_DHWSTATE_H
#define AQUAMQTT_DHWSTATE_H

#include <Arduino.h>

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
        , mHasMessage67(false)
        , mHasMessage193(false)
        , mHasMessage194(false)
        , mMessage194{ 0 }
        , mMessage193{ 0 }
        , mMessage67{ 0 }
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
        if (frameId == 194 && payloadLength == 35 && memcmp(mMessage194, payload, payloadLength) != 0)
        {
            memcpy(mMessage194, payload, 35);
            mHasMessage194 = true;

            if (mNotify != nullptr)
            {
                xTaskNotifyIndexed(mNotify, 0, (1UL << 8UL), eSetBits);
            }
        }
        else if (frameId == 193 && payloadLength == 37 && memcmp(mMessage193, payload, payloadLength) != 0)
        {
            memcpy(mMessage193, payload, 37);
            mHasMessage193 = true;

            if (mNotify != nullptr)
            {
                xTaskNotifyIndexed(mNotify, 0, (1UL << 7UL), eSetBits);
            }
        }
        else if (frameId == 67 && payloadLength == 31 && memcmp(mMessage67, payload, payloadLength) != 0)
        {
            memcpy(mMessage67, payload, 31);
            mHasMessage67 = true;

            if (mNotify != nullptr)
            {
                xTaskNotifyIndexed(mNotify, 0, (1UL << 6UL), eSetBits);
            }
        }

        xSemaphoreGive(mMutex);

        delete[] payload;
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
        if (frameId == 194 && mHasMessage194)
        {
            memcpy(buffer, mMessage194, 35);
            handled = true;
        }
        else if (frameId == 193 && mHasMessage193)
        {
            memcpy(buffer, mMessage193, 37);
            handled = true;
        }
        else if (frameId == 67 && mHasMessage67)
        {
            memcpy(buffer, mMessage67, 31);
            handled = true;
        }

        xSemaphoreGive(mMutex);

        return handled;
    }

private:
    TaskHandle_t mNotify;

    SemaphoreHandle_t mMutex;

    bool mHasMessage194;
    bool mHasMessage193;
    bool mHasMessage67;

    uint8_t mMessage194[35];
    uint8_t mMessage193[37];
    uint8_t mMessage67[31];

    BufferStatistics mHmiStats;
    BufferStatistics mMainStats;
    BufferStatistics mListenerStats;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_DHWSTATE_H
