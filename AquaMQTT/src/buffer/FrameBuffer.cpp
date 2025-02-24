#include "buffer/FrameBuffer.h"

#include <config/Configuration.h>

#include "state/DHWState.h"

#define FRAME_ID_LEN_BYTES 1

using namespace aquamqtt;
using namespace aquamqtt::message;

FrameBuffer::FrameBuffer(const FrameBufferChannel channel)
    : mLockedProtocol(PROTOCOL_UNKNOWN)
    , mLockedChecksum(CHECKSUM_TYPE_UNKNOWN)
    , mTransferBuffer{}
    , mChannel(channel)
    , mDroppedCount(0)
    , mCRCFailCount(0)
    , mUnhandledCount(0)
    , mHandledCount(0)
    , mPreviousFrameId(0)
    , mLastValidFrameTimestamp(0)
{
    if (channel == FrameBufferChannel::CH_LISTENER)
    {
        mHandle194 = true;
        mHandle193 = true;
        mHandle67  = true;
        mHandle74  = true;
        mHandle217 = true;
    }
    else if (channel == FrameBufferChannel::CH_HMI)
    {
        mHandle194 = true;
        mHandle193 = false;
        mHandle67  = false;
        mHandle74  = false;
        mHandle217 = false;
    }
    else
    {
        mHandle194 = false;
        mHandle193 = true;
        mHandle67  = true;
        mHandle74  = true;
        mHandle217 = true;
    }
}

int FrameBuffer::pushByte(const int val)
{
    // if buffer is full, we drop elements from the beginning
    if (mBuffer.isFull())
    {
        int retVal;
        mBuffer.pop(retVal);

        // allow debuggability if we unexceptionally drop something from the ringbuffer
        if (config::DEBUG_PUBLISH_DROPPED_MESSAGES)
        {
            DHWState::getInstance().debugAddToDropBuffer(mChannel, retVal);
        }

        mDroppedCount++;
    }

    // push new element to the back
    mBuffer.push(val);

    // see if ring buffer contains a full element
    return handleFrame();
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
        const int payloadLength = mBuffer[1];

        // determine the checksum size, if we do not ḱnow yet, await two bytes
        uint8_t crcLen;
        switch (mLockedChecksum)
        {
            case CHECKSUM_TYPE_XOR:
                crcLen = 1;
                break;
            default:
            case CHECKSUM_TYPE_CRC16:
            case CHECKSUM_TYPE_UNKNOWN:
                crcLen = 2;
                break;
        }

        // wait until buffer holds a complete frame.
        if (mBuffer.size() < FRAME_ID_LEN_BYTES + payloadLength + crcLen)
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

        ProtocolVersion protocolVersion = mLockedProtocol;
        if (protocolVersion == PROTOCOL_UNKNOWN)
        {
            protocolVersion = getVersionByIdentifier(frameId, payloadLength);
        }

        bool crcResult = false;
        if (protocolVersion == PROTOCOL_NEXT || protocolVersion == PROTOCOL_ODYSSEE)
        {
            // move checksum out of ringbuffer
            int actualChecksum;
            mBuffer.pop(actualChecksum);

            int desiredChecksum = generateXorChecksum(mTransferBuffer, payloadLength);

            crcResult = actualChecksum == desiredChecksum;

            if (crcResult)
            {
                mLockedChecksum = CHECKSUM_TYPE_XOR;
            }
        }
        else
        {
            // legacy protocol either uses XOR or CRC16 checksum
            if (mLockedChecksum == CHECKSUM_TYPE_UNKNOWN)
            {
                // try xor
                int crcVal1;
                mBuffer.pop(crcVal1);
                int xorChecksum = generateXorChecksum(mTransferBuffer, payloadLength);

                if (crcVal1 == xorChecksum)
                {
                    mLockedChecksum = CHECKSUM_TYPE_XOR;
                    crcResult       = true;
                }
                else
                {
                    // try crc16
                    int crcVal2;
                    mBuffer.pop(crcVal2);

                    uint16_t desiredCRC = crcVal1 << 8 | crcVal2;
                    uint16_t actualCRC  = mCRC.ccitt(mTransferBuffer, payloadLength);

                    if (desiredCRC == actualCRC)
                    {
                        mLockedChecksum = CHECKSUM_TYPE_CRC16;
                        crcResult       = true;
                    }
                    else
                    {
                        crcResult = false;
                    }
                }
            }
            else if (mLockedChecksum == CHECKSUM_TYPE_XOR)
            {
                int actualChecksum;
                mBuffer.pop(actualChecksum);

                int desiredChecksum = generateXorChecksum(mTransferBuffer, payloadLength);

                crcResult = actualChecksum == desiredChecksum;
            }
            else if (mLockedChecksum == CHECKSUM_TYPE_CRC16)
            {
                // move crc out of ringbuffer
                int crcVal1;
                mBuffer.pop(crcVal1);

                // move crc out of ringbuffer
                int crcVal2;
                mBuffer.pop(crcVal2);

                uint16_t desiredCRC = crcVal1 << 8 | crcVal2;
                uint16_t actualCRC  = mCRC.ccitt(mTransferBuffer, payloadLength);
                crcResult           = desiredCRC == actualCRC;
            }
        }

        // completed and valid frame, move complete frame and ownership to frame handler
        if (crcResult)
        {
            if (mChannel == FrameBufferChannel::CH_LISTENER && config::DEBUG_PUBLISH_FRAME_TIMING_IN_LISTENER_MODE)
            {
                // determine protocol frequencies without signal analyzer
                if (mPreviousFrameId != 0)
                {
                    DHWState::getInstance().debugSaveFrameTiming(mPreviousFrameId, frameId, millis() - mLastValidFrameTimestamp);
                }
                mPreviousFrameId         = frameId;
                mLastValidFrameTimestamp = millis();
            }

            // lock the protocol version, since we found a valid message and checksum is good
            if (mLockedProtocol != protocolVersion)
            {
                mLockedProtocol = protocolVersion;
                DHWState::getInstance().setVersion(protocolVersion);
                DHWState::getInstance().setChecksumType(mLockedChecksum);
            }

            if ((frameId == HMI_MESSAGE_IDENTIFIER && mHandle194) || (frameId == MAIN_MESSAGE_IDENTIFIER && mHandle193)
                || (frameId == ENERGY_MESSAGE_IDENTIFIER && mHandle67)
                || (frameId == ERROR_MESSAGE_IDENTIFIER && mHandle74)
                || (frameId == EXTRA_MESSAGE_IDENTIFIER && mHandle217))
            {
                DHWState::getInstance().storeFrame(frameId, payloadLength, mTransferBuffer);
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

// wait until ringbuffer starts with the beginning of a potential message
bool FrameBuffer::isSync()
{
    switch (mLockedProtocol)
    {
        case PROTOCOL_LEGACY:
            return ((mBuffer[0] == HMI_MESSAGE_IDENTIFIER && mBuffer[1] == HMI_MESSAGE_LENGTH_LEGACY)
                    || (mBuffer[0] == MAIN_MESSAGE_IDENTIFIER && mBuffer[1] == MAIN_MESSAGE_LENGTH_LEGACY)
                    || (mBuffer[0] == ENERGY_MESSAGE_IDENTIFIER && mBuffer[1] == ENERGY_MESSAGE_LENGTH_LEGACY)
                    || (mBuffer[0] == ERROR_MESSAGE_IDENTIFIER && mBuffer[1] == ERROR_MESSAGE_LENGTH_LEGACY));
        case PROTOCOL_NEXT:
            return ((mBuffer[0] == HMI_MESSAGE_IDENTIFIER && mBuffer[1] == HMI_MESSAGE_LENGTH_NEXT)
                    || (mBuffer[0] == MAIN_MESSAGE_IDENTIFIER && mBuffer[1] == MAIN_MESSAGE_LENGTH_NEXT)
                    || (mBuffer[0] == ENERGY_MESSAGE_IDENTIFIER && mBuffer[1] == ENERGY_MESSAGE_LENGTH_NEXT)
                    || (mBuffer[0] == ERROR_MESSAGE_IDENTIFIER && mBuffer[1] == ERROR_MESSAGE_LENGTH_NEXT));
        case PROTOCOL_ODYSSEE:
            return ((mBuffer[0] == HMI_MESSAGE_IDENTIFIER && mBuffer[1] == HMI_MESSAGE_LENGTH_ODYSSEE)
                    || (mBuffer[0] == MAIN_MESSAGE_IDENTIFIER && mBuffer[1] == MAIN_MESSAGE_LENGTH_ODYSSEE)
                    || (mBuffer[0] == ENERGY_MESSAGE_IDENTIFIER && mBuffer[1] == ENERGY_MESSAGE_LENGTH_ODYSSEE)
                    || (mBuffer[0] == ERROR_MESSAGE_IDENTIFIER && mBuffer[1] == ERROR_MESSAGE_LENGTH_ODYSSEE)
                    || (mBuffer[0] == EXTRA_MESSAGE_IDENTIFIER && mBuffer[1] == EXTRA_MESSAGE_LENGTH_ODYSSEE));

        default:
        case PROTOCOL_UNKNOWN:
            return ((mBuffer[0] == HMI_MESSAGE_IDENTIFIER && mBuffer[1] == HMI_MESSAGE_LENGTH_NEXT)
                    || (mBuffer[0] == MAIN_MESSAGE_IDENTIFIER && mBuffer[1] == MAIN_MESSAGE_LENGTH_NEXT)
                    || (mBuffer[0] == ENERGY_MESSAGE_IDENTIFIER && mBuffer[1] == ENERGY_MESSAGE_LENGTH_NEXT)
                    || (mBuffer[0] == ERROR_MESSAGE_IDENTIFIER && mBuffer[1] == ERROR_MESSAGE_LENGTH_NEXT)
                    || (mBuffer[0] == HMI_MESSAGE_IDENTIFIER && mBuffer[1] == HMI_MESSAGE_LENGTH_LEGACY)
                    || (mBuffer[0] == MAIN_MESSAGE_IDENTIFIER && mBuffer[1] == MAIN_MESSAGE_LENGTH_LEGACY)
                    || (mBuffer[0] == ENERGY_MESSAGE_IDENTIFIER && mBuffer[1] == ENERGY_MESSAGE_LENGTH_LEGACY)
                    || (mBuffer[0] == ERROR_MESSAGE_IDENTIFIER && mBuffer[1] == ERROR_MESSAGE_LENGTH_LEGACY)
                    || mBuffer[0] == HMI_MESSAGE_IDENTIFIER && mBuffer[1] == HMI_MESSAGE_LENGTH_ODYSSEE)
                   || (mBuffer[0] == MAIN_MESSAGE_IDENTIFIER && mBuffer[1] == MAIN_MESSAGE_LENGTH_ODYSSEE)
                   || (mBuffer[0] == ENERGY_MESSAGE_IDENTIFIER && mBuffer[1] == ENERGY_MESSAGE_LENGTH_ODYSSEE)
                   || (mBuffer[0] == ERROR_MESSAGE_IDENTIFIER && mBuffer[1] == ERROR_MESSAGE_LENGTH_ODYSSEE)
                   || (mBuffer[0] == EXTRA_MESSAGE_IDENTIFIER && mBuffer[1] == EXTRA_MESSAGE_LENGTH_ODYSSEE);
    }
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
