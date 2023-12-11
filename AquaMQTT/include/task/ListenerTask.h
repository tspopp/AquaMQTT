#ifndef AQUAMQTT_LISTENERTASK_H
#define AQUAMQTT_LISTENERTASK_H

#include "buffer/FrameBuffer.h"

namespace aquamqtt
{

class ListenerTask
{
public:
    ListenerTask();

    virtual ~ListenerTask() = default;

    void spawn();

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    void setup();

    void loop();

private:
    FrameBuffer mBuffer;

    unsigned long mLastStatisticsUpdate;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_LISTENERTASK_H
