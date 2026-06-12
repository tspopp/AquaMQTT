#include "Frame.h"

#include <cstring>

#include "util.h"
#include "constants.h"
#include "protocol.h"

namespace aquamqtt {

#define HMI_TASK_NAME "hmi"
#define CONTROLLER_TASK_NAME "controller"
#define LISTENER_TASK_NAME "listener"

const char* channel_name(FrameChannel channel) {
    switch(channel) {
    case FrameChannel::CH_HMI:
        return HMI_TASK_NAME;
    case FrameChannel::CH_MAIN:
        return CONTROLLER_TASK_NAME;
    case FrameChannel::CH_LISTENER:
        return LISTENER_TASK_NAME;
    default:
        return "unknown";
    }
}

Frame::Frame(const FrameChannel channel, const uint8_t *_buffer, const uint8_t _buffer_size)
    : mChannel(channel)
    , buffer_size(_buffer_size <= MAX_FRAME_SIZE ? _buffer_size : 0)
{
    memcpy(buffer, _buffer, _buffer_size);
}

Frame::Frame(const Frame& other)
    : mChannel(other.mChannel)
    , buffer_size(other.buffer_size <= MAX_FRAME_SIZE ? other.buffer_size : 0)
{
    memcpy(buffer, other.buffer, other.buffer_size);
}

const char* Frame::getChannelName() const {
    return channel_name(mChannel);
}

const uint8_t* Frame::payload() const {
    return &buffer[HEADER_LENGTH + 1];
}

int Frame::payload_size() const {
    if (buffer_size <= HEADER_LENGTH + 2) {
        return 0;
    } else {
        return buffer_size - 2 - 1 - HEADER_LENGTH;
    }
}

uint64_t Frame::getHeaderValue() const
{
    uint64_t value = 0;
    for (int i=0 ; i<HEADER_LENGTH ; i++) {
        value = value << 8 | buffer[i];
    }
    return value;
}

const std::string Frame::getBufferAsString() const {
    char hex[MAX_FRAME_SIZE * 2 + 1];
    for (int i=0 ; i<buffer_size ; i++) {
        sprintf(hex + 2*i, "%02X", buffer[i]);
    }
    std::string result = hex;
    return result;
}

void Frame::replace_payload(uint8_t payload[])
{
    memcpy(buffer+HEADER_LENGTH+1, payload, payload_size());

    uint16_t crc = calculate_crc(buffer, buffer_size-2);
    buffer[buffer_size-1] = crc >> 8;
    buffer[buffer_size-2] = crc & 0xff;
    
    is_modified = true;
}

Frame& Frame::operator=(const Frame& other) {
    if (this == &other) {
        return *this;
    }

    mChannel = other.mChannel;
    buffer_size = other.buffer_size;
    memcpy(buffer, other.buffer, other.buffer_size);

    return *this;
}

bool Frame::operator==(const Frame& other) const {
    if (mChannel != other.mChannel || buffer_size != other.buffer_size) {
        return false;
    }
    if (memcmp(buffer, other.buffer, buffer_size)) {
        return false;
    }
    return true;
}

bool Frame::operator!=(const Frame& other) const {
    return !(*this == other);
}

}
