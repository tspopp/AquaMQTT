#ifndef AQUAMQTT_FRAMEBUFFER_H
#define AQUAMQTT_FRAMEBUFFER_H

#include <FastCRC.h>
#include <RingBuf.h>

#include "message/MessageConstants.h"

class FrameBuffer
{
public:
    explicit FrameBuffer(bool handle194, bool handle193, bool handle67, bool handle74);

    ~FrameBuffer() = default;

    int pushByte(int val);

    uint64_t getDroppedCount() const;

    uint64_t getCRCFailedCount() const;

    uint64_t getUnhandledCount() const;

    uint64_t getHandledCount() const;

private:
    int handleFrame();

    bool isSync();

    RingBuf<int, aquamqtt::message::HEATPUMP_MAX_FRAME_LENGTH> mBuffer;
    uint8_t mTransferBuffer[aquamqtt::message::HEATPUMP_MAX_FRAME_LENGTH];

    FastCRC16 mCRC;

    bool mHandle194;
    bool mHandle193;
    bool mHandle67;
    bool mHandle74;

    uint64_t mDroppedCount;
    uint64_t mCRCFailCount;
    uint64_t mUnhandledCount;
    uint64_t mHandledCount;
};

#endif  // AQUAMQTT_FRAMEBUFFER_H
