#ifndef AQUAMQTT_HMITASK_H
#define AQUAMQTT_HMITASK_H

#include "buffer/FrameBuffer.h"
#include "message/MessageConstants.h"

namespace aquamqtt
{

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

private:
    FrameBuffer   mBuffer;
    unsigned long mLastStatisticsUpdate;
    uint8_t       mTransferBuffer[message::HEATPUMP_MAX_FRAME_LENGTH];
    FastCRC16     mCRC;
    uint64_t      mMessagesSent;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_HMITASK_H
