#include "buffer/ModbusBuffer.h"

#include <mqtt/MQTTDefinitions.h>

using namespace aquamqtt::modbus;

ModbusBuffer::ModbusBuffer(MQTTClient* mqtt_client)
    : mMQTTClient(mqtt_client)
    , mTransferBuffer{}
    , mDroppedCount(0)
    , mCRCFailCount(0)
    , mUnhandledCount(0)
    , mHandledCount(0)
    , mPreviousFrameId(0)
    , mLastValidFrameTimestamp(0)
{
}

int ModbusBuffer::pushByte(const int val)
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
    return handleFrame();
}

int ModbusBuffer::handleFrame()
{
    // sanity check, smallest header is of length 7 (2 bytes magic, 3 bytes address, 2 bytes crc)
    if (mBuffer.size() < 7)
    {
        return 0;
    }

    // if buffer begins with a known frame magic identifier
    if (isSync())
    {
        // FIXME: this will cause calculation of an new crc for every incoming byte on a message
        // FIXME: avoid copy
        uint8_t header[5];
        header[0] = mBuffer[0];
        header[1] = mBuffer[1];
        header[2] = mBuffer[2];
        header[3] = mBuffer[3];
        header[4] = mBuffer[4];

        // TODO: check if this is a header without moving data out of ringbuffer
        // uint16_t actualCRC  = mCRC.ccitt(reinterpret_cast<uint8_t*>(&mBuffer[0]), 5);
        uint16_t actualCRC  = mCRC.modbus(header, 5);
        uint16_t desiredCRC = mBuffer[6] << 8 | mBuffer[5];

        bool isHeader = desiredCRC == actualCRC;

        if (isHeader)
        {
            Serial.println("pure header, ignoring");
            // we ignore headers, pop data out of ringbuffer
            for (int i = 0; i < 7; ++i)
            {
                int val;
                mBuffer.pop(val);
            }
            return 0;
        }

        // not an header, interpret lenght field
        const int payloadLength = mBuffer[5];

        // wait until buffer holds a complete frame.
        int totalLength = 6 + payloadLength;
        if (mBuffer.size() < totalLength + 2)
        {
            return 0;
        }

        // move payload out of ringbuffer
        for (int i = 0; i < totalLength; i++)
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

        uint16_t desiredCRCMessage = crcVal2 << 8 | crcVal1;
        uint16_t actualCRCMessage  = mCRC.modbus(mTransferBuffer, totalLength);

        // completed and valid frame, move complete frame and ownership to frame handler
        if (desiredCRCMessage == actualCRCMessage)
        {
            Serial.println("valid message");
            Serial.print("slave: ");
            Serial.print(mTransferBuffer[0]);
            Serial.print(", fn code: ");
            auto functionCode = mTransferBuffer[1];
            Serial.print(functionCode);
            Serial.print(", p01: ");
            auto parameter01 = mTransferBuffer[2];
            Serial.print(parameter01);
            Serial.print(", p02: ");
            auto parameter02 = mTransferBuffer[3];
            Serial.print(parameter02);
            auto subfn = mTransferBuffer[4];
            Serial.print(", subfn: ");
            Serial.print(subfn);
            auto length = mTransferBuffer[5];
            Serial.print(", ln: ");
            Serial.println(length);

            // FIXME: prototype currently handles all / later we migth distinguish between "from HMI" or "from Main"
            if (true)
            {
                sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%s%d/%d/%d/%d/%s", config::mqttPrefix,
                mqtt::BASE_TOPIC, functionCode, parameter01, parameter02, subfn, mqtt::DEBUG);
                toHexStr(mTransferBuffer, length, reinterpret_cast<char*>(mPayloadBuffer));
                mMQTTClient->publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
                mHandledCount++;
            }
            else
            {
                mUnhandledCount++;
            }

            return 1;
            // FIXME: frameId
            // return frameId;
        }
        mCRCFailCount++;
        return 0;
    }
    return 0;
}

// wait until ringbuffer starts with the beginning of a potential message
// check if ringbuffer starts with either 0x01 0x64 OR 0x01 0x65
bool ModbusBuffer::isSync()
{
    return (mBuffer[0] == 0x01 && mBuffer[1] == 0x64) || (mBuffer[0] == 0x01 && mBuffer[1] == 0x65);
}

uint64_t ModbusBuffer::getDroppedCount() const
{
    return mDroppedCount;
}

uint64_t ModbusBuffer::getCRCFailedCount() const
{
    return mCRCFailCount;
}

uint64_t ModbusBuffer::getUnhandledCount() const
{
    return mUnhandledCount;
}

uint64_t ModbusBuffer::getHandledCount() const
{
    return mHandledCount;
}

void ModbusBuffer::toHexStr(uint8_t* data, uint8_t data_len, char* buffer)
{
    const size_t num_bytes = data_len / sizeof(uint8_t);
    for (size_t i = 0; i < num_bytes; i++)
    {
        sprintf(&buffer[i * 2], "%02X", data[i]);
    }
    buffer[num_bytes * 2] = '\0';
}