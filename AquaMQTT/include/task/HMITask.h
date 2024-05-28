#ifndef AQUAMQTT_HMITASK_H
#define AQUAMQTT_HMITASK_H

#include "buffer/FrameBuffer.h"
#include "message/MessageConstants.h"

namespace aquamqtt
{

enum class HMITaskState
{
    REQUESTING_194,
    SLEEP_194,
    SENDING_67,
    SLEEP_67,
    SENDING_193,
    SLEEP_193,
    SENDING_74,
    SLEEP_74,
};

class HMITask
{
public:
    HMITask();

    virtual ~HMITask() = default;

    void spawn();

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    void setup();

    void loop();

    static void flushReadBuffer();

    void sendMessage67();
    void sendMessage193();
    void sendMessage74();

private:
    FrameBuffer   mBuffer;
    unsigned long mLastStatisticsUpdate;
    unsigned long mLastMessageSent;
    uint8_t       mLastEmittedRequestId;
    uint8_t       mTransferBuffer[message::HEATPUMP_MAX_FRAME_LENGTH];
    FastCRC16     mCRC;
    uint64_t      mMessagesSent;
    HMITaskState  mState;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_HMITASK_H
