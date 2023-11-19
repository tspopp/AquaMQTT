#ifndef AQUAMQTT_CONTROLLERTASK_H
#define AQUAMQTT_CONTROLLERTASK_H

#include <Arduino.h>

#include "buffer/FrameBuffer.h"

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
    uint8_t       mTransferBuffer[40];
    FastCRC16     mCRC;
    uint64_t      mMessagesSent;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_CONTROLLERTASK_H
