#ifndef AQUAMQTT_FRAMEBUFFER_H
#define AQUAMQTT_FRAMEBUFFER_H

#include <FastCRC.h>
#include <RingBuf.h>

#define HEATPUMP_MAX_FRAME_LENGTH 40

class FrameBuffer
{
public:
    explicit FrameBuffer(bool handle194, bool handle193, bool handle67, std::string name);

    ~FrameBuffer() = default;

    int pushByte(int val);

    uint64_t getDroppedCount() const;

    uint64_t getCRCFailedCount() const;

    uint64_t getUnhandledCount() const;

    uint64_t getHandledCount() const;

private:
    int handleFrame();

    bool isSync();

    RingBuf<int, HEATPUMP_MAX_FRAME_LENGTH> mBuffer;

    FastCRC16 mCRC;

    bool        mHandle194;
    bool        mHandle193;
    bool        mHandle67;
    std::string mName;

    uint64_t mDroppedCount;
    uint64_t mCRCFailCount;
    uint64_t mUnhandledCount;
    uint64_t mHandledCount;
};

#endif  // AQUAMQTT_FRAMEBUFFER_H
