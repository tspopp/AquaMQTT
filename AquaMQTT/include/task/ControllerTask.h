#ifndef AQUAMQTT_CONTROLLERTASK_H
#define AQUAMQTT_CONTROLLERTASK_H

#include "buffer/FrameBuffer.h"
#include "message/MessageConstants.h"

namespace aquamqtt
{

enum class ControllerTaskState
{
    AWAITING_67,
    AWAITING_193,
    CHECK_FOR_HMI_TRIGGER,
    AWAITING_74,
};

class ControllerTask
{
public:
    ControllerTask();

    virtual ~ControllerTask() = default;

    void spawn();

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    void setup();

    void loop();

    void sendMessage194();

    static void flushReadBuffer();

private:
    FrameBuffer         mBuffer;
    unsigned long       mLastStatisticsUpdate;
    uint8_t             mTransferBuffer[message::HEATPUMP_MAX_FRAME_LENGTH];
    FastCRC16           mCRC;
    uint64_t            mMessagesSent;
    ControllerTaskState mState;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_CONTROLLERTASK_H
