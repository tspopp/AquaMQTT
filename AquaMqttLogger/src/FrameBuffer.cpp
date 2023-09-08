#include "FrameBuffer.h"

#define FRAME_ID_LEN_BYTES 1
#define CRC_LEN_BYTES      2

FrameBuffer::FrameBuffer(FrameHandler* handler) : mFrameHandler(handler)
{
}

void FrameBuffer::pushByte(int val)
{
    // if buffer is full, we drop elements from the beginning
    if (mBuffer.isFull())
    {
        int retVal;
        mBuffer.pop(retVal);
    }

    // push new element to the back
    mBuffer.push(val);

    // see if ring buffer contains a full element
    handleFrame();
}

void FrameBuffer::handleFrame()
{
    // sanity check, buffer must contain at least two elements for further processing
    if (mBuffer.size() < 2)
    {
        return;
    }

    // if buffer begins with a known frame magic identifier
    if (isSync())
    {
        // access expected payload length
        int payloadLength = mBuffer[1];

        // wait until buffer holds a complete frame.
        if (mBuffer.size() < FRAME_ID_LEN_BYTES + payloadLength + CRC_LEN_BYTES)
        {
            return;
        }

        // allocate buffer memory
        auto* payloadBuffer = new uint8_t[payloadLength]();

        // move frameId out of ringbuffer
        int frameId;
        mBuffer.pop(frameId);

        // move payload out of ringbuffer
        for (int i = 0; i < payloadLength; i++)
        {
            int retVal;
            mBuffer.pop(retVal);
            payloadBuffer[i] = retVal;
        }

        // move crc out of ringbuffer
        int crcVal1;
        mBuffer.pop(crcVal1);

        // move crc out of ringbuffer
        int crcVal2;
        mBuffer.pop(crcVal2);

        uint16_t desiredCRC = crcVal1 << 8 | crcVal2;
        uint16_t actualCRC  = mCRC.ccitt(payloadBuffer, payloadLength);

        // completed and valid frame, move complete frame and ownership to frame handler
        if (desiredCRC == actualCRC)
        {
            mFrameHandler->handleFrame(frameId, payloadLength, payloadBuffer);
        }
        else
        {
            delete[] payloadBuffer;
            Serial.print(F("WARN: CRC Mismatch, Dropping Frame: "));
            Serial.println(frameId);
        }
    }
}

int MAGIC_SYNC_MESSAGES[3][2] = {
    { 194, 35 },
    { 193, 37 },
    { 67, 31 },
};

// wait until ringbuffer starts with the beginning of a potential message
bool FrameBuffer::isSync()
{
    if (mBuffer[0] == MAGIC_SYNC_MESSAGES[0][0] && mBuffer[1] == MAGIC_SYNC_MESSAGES[0][1])
    {
        return true;
    }
    else if (mBuffer[0] == MAGIC_SYNC_MESSAGES[1][0] && mBuffer[1] == MAGIC_SYNC_MESSAGES[1][1])
    {
        return true;
    }
    else if (mBuffer[0] == MAGIC_SYNC_MESSAGES[2][0] && mBuffer[1] == MAGIC_SYNC_MESSAGES[2][1])
    {
        return true;
    }
    else
    {
        return false;
    }
}