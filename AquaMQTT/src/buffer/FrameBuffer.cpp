#include "buffer/FrameBuffer.h"

#include "state/DHWState.h"

#define FRAME_ID_LEN_BYTES 1
#define CRC_LEN_BYTES      1

using namespace aquamqtt;
using namespace aquamqtt::message;

FrameBuffer::FrameBuffer(bool handle194, bool handle193, bool handle67, bool handle74)
    : mHandle194(handle194)
    , mHandle193(handle193)
    , mHandle67(handle67)
    , mHandle74(handle74)
    , mDroppedCount(0)
    , mCRCFailCount(0)
    , mUnhandledCount(0)
    , mHandledCount(0)
    , mTransferBuffer{ 0 }
    , mLockedProtocol(PROTOCOL_UNKNOWN)
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

        ProtocolVersion protocolVersion = mLockedProtocol;
        if (protocolVersion == PROTOCOL_UNKNOWN)
        {
            protocolVersion = getVersionByIdentifier(frameId, payloadLength);
        }

        bool crcResult = false;
        if (protocolVersion == ProtocolVersion::PROTOCOL_NEXT)
        {
            // move checksum out of ringbuffer
            int checksum;
            mBuffer.pop(checksum);

            //            int32_t sum = frameId;
            //            if (frameId == 194)
            //            {
            //                Serial.print("msg ");
            //                Serial.print(frameId);
            //                Serial.print(" checksum is ");
            //                Serial.print(checksum);
            //                Serial.print(" ");
            //            }
            //            for (size_t i = 1; i < payloadLength; i++)
            //            {
            //                auto val = mTransferBuffer[i];
            //
            //                // even
            //                if (val % 2 == 0)
            //                {
            //                    sum += mTransferBuffer[i];
            //                }
            //                else
            //                {
            //                    sum -= mTransferBuffer[i];
            //                }
            //
            //                if (frameId == 194)
            //                {
            //                    Serial.print(mTransferBuffer[i]);
            //                }
            //            }
            //            if (frameId == 194)
            //            {
            //                Serial.println();
            //                Serial.print("msg: ");
            //                Serial.print(frameId);
            //                Serial.print("sum: ");
            //                Serial.println(sum);
            //                Serial.print("sum + checksum ");
            //                Serial.println(sum + checksum);
            //                Serial.print("sum - checksum ");
            //                Serial.println(sum - checksum);
            //                Serial.println("sum mod 256/255");
            //                Serial.println(sum % 256);
            //                Serial.println(sum % 255);
            //                Serial.println(sum % 256 + checksum);
            //                Serial.println((sum % 256) - checksum);
            //                Serial.println(sum % 255 + checksum);
            //                Serial.println((sum % 255) - checksum);
            //            }

            // FIXME: analyze checksum calulation
            crcResult = true;
        }
        else
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

        // completed and valid frame, move complete frame and ownership to frame handler
        if (crcResult)
        {
            // lock the protocol version, since we found a valid message and checksum is good
            if (mLockedProtocol != protocolVersion)
            {
                mLockedProtocol = protocolVersion;
                aquamqtt::DHWState::getInstance().setVersion(protocolVersion);
            }

            if ((frameId == HMI_MESSAGE_IDENTIFIER && mHandle194) || (frameId == MAIN_MESSAGE_IDENTIFIER && mHandle193)
                || (frameId == ENERGY_MESSAGE_IDENTIFIER && mHandle67)
                || (frameId == ERROR_MESSAGE_IDENTIFIER && mHandle74))
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
                    );
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
