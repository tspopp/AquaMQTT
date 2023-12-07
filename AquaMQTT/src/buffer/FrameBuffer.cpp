#include "buffer/FrameBuffer.h"

#include "state/DHWState.h"

#define FRAME_ID_LEN_BYTES 1
#define CRC_LEN_BYTES      2

FrameBuffer::FrameBuffer(bool handle194, bool handle193, bool handle67, std::string name)
    : mHandle194(handle194)
    , mHandle193(handle193)
    , mHandle67(handle67)
    , mName(std::move(name))
    , mDroppedCount(0)
    , mCRCFailCount(0)
    , mUnhandledCount(0)
    , mHandledCount(0)
    , mTransferBuffer{ 0 }
{
}

int FrameBuffer::pushByte(int val)
{
    // if buffer is full, we drop elements from the beginning
    if (mBuffer.isFull())
    {
        int retVal;
        mBuffer.pop(retVal);
        mDroppedCount++;
    }

    // push new element to the back
    mBuffer.push(val);

    // see if ring buffer contains a full element
    int result = handleFrame();

    return result;
}

int FrameBuffer::handleFrame()
{
    // sanity check, buffer must contain at least two elements for further processing
    if (mBuffer.size() < 2)
    {
        return 0;
    }

    // if buffer begins with a known frame magic identifier
    if (isSync())
    {
        // access expected payload length
        int payloadLength = mBuffer[1];

        // wait until buffer holds a complete frame.
        if (mBuffer.size() < FRAME_ID_LEN_BYTES + payloadLength + CRC_LEN_BYTES)
        {
            return 0;
        }

        // move frameId out of ringbuffer
        int frameId;
        mBuffer.pop(frameId);

        // move payload out of ringbuffer
        for (int i = 0; i < payloadLength; i++)
        {
            int retVal;
            mBuffer.pop(retVal);
            mTransferBuffer[i] = retVal;
        }

        // move crc out of ringbuffer
        int crcVal1;
        mBuffer.pop(crcVal1);

        // move crc out of ringbuffer
        int crcVal2;
        mBuffer.pop(crcVal2);

        uint16_t desiredCRC = crcVal1 << 8 | crcVal2;
        uint16_t actualCRC  = mCRC.ccitt(mTransferBuffer, payloadLength);

        // completed and valid frame, move complete frame and ownership to frame handler
        if (desiredCRC == actualCRC)
        {
            if ((frameId == 194 && mHandle194) || (frameId == 193 && mHandle193) || (frameId == 67 && mHandle67))
            {
                aquamqtt::DHWState::getInstance().storeFrame(frameId, payloadLength, mTransferBuffer);
                mHandledCount++;
            }
            else
            {
                mUnhandledCount++;
            }
            return frameId;
        }
        mCRCFailCount++;
        return 0;
    }
    return 0;
}

int MAGIC_SYNC_MESSAGES[3][2] = {
    { 194, 35 },
    { 193, 37 },
    { 67, 31 },
};

// wait until ringbuffer starts with the beginning of a potential message
bool FrameBuffer::isSync()
{
    return ((mBuffer[0] == MAGIC_SYNC_MESSAGES[0][0] && mBuffer[1] == MAGIC_SYNC_MESSAGES[0][1])
            || (mBuffer[0] == MAGIC_SYNC_MESSAGES[1][0] && mBuffer[1] == MAGIC_SYNC_MESSAGES[1][1])
            || (mBuffer[0] == MAGIC_SYNC_MESSAGES[2][0] && mBuffer[1] == MAGIC_SYNC_MESSAGES[2][1]));
}
uint64_t FrameBuffer::getDroppedCount() const
{
    return mDroppedCount;
}
uint64_t FrameBuffer::getCRCFailedCount() const
{
    return mCRCFailCount;
}
uint64_t FrameBuffer::getUnhandledCount() const
{
    return mUnhandledCount;
}
uint64_t FrameBuffer::getHandledCount() const
{
    return mHandledCount;
}
