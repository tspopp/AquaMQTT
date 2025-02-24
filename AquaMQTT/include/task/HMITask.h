#ifndef AQUAMQTT_HMITASK_H
#define AQUAMQTT_HMITASK_H

#include "buffer/FrameBuffer.h"
#include "message/MessageConstants.h"

namespace aquamqtt
{

enum class HMITaskState
{
    REQUEST_194,
    SLEEP_194,
    SEND_67,
    SLEEP_67,
    SEND_74POST,
    SLEEP_74POST,
    SEND_193,
    SLEEP_193,
    SEND_217,
    SLEEP_217,
    SEND_74PRE,
    SLEEP_74PRE
};

class HMITask final
{
public:
    HMITask();

    ~HMITask() = default;

    void spawn();

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    void setup();
    void awaitStateChangeTo(HMITaskState state, bool performStateChange);

    void loop();

    static void flushReadBuffer();

    void sendMessage67();
    void sendMessage193();
    void sendMessage74();
    void sendMessage217();

    FrameBuffer              mBuffer;
    unsigned long            mLastStatisticsUpdate;
    unsigned long            mLastMessageSent;
    uint8_t                  mLastEmittedRequestId;
    uint8_t                  mTransferBuffer[message::HEATPUMP_MAX_FRAME_LENGTH];
    FastCRC16                mCRC;
    uint64_t                 mMessagesSent;
    HMITaskState             mState;
    message::ProtocolVersion mVersion;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_HMITASK_H
