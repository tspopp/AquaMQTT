#ifndef AQUAMQTT_CONTROLLERTASK_H
#define AQUAMQTT_CONTROLLERTASK_H

#include "buffer/FrameBuffer.h"
#include "message/MessageConstants.h"

namespace aquamqtt
{

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

private:
    bool          mFlagSeen67;
    bool          mFlagSeen193;
    FrameBuffer   mBuffer;
    unsigned long mLastStatisticsUpdate;
    uint8_t       mTransferBuffer[message::HEATPUMP_MAX_FRAME_LENGTH];
    FastCRC16     mCRC;
    uint64_t      mMessagesSent;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_CONTROLLERTASK_H
