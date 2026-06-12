/**
 * A frame is formatted as follows:
 * - byte 0: always 0x01
 * - byte 1: 0x64 or 0x65
 * - byte 2-5: message type
 * - byte 6 (optional): payload length
 * - byte 7+ (optional): payload
 * - last two bytes: checksum
 */
#ifndef AQUAMQTT_FRAME_H
#define AQUAMQTT_FRAME_H

#include <cstdint>
#include <string>

#include "constants.h"

// the number of bytes in the header
#define HEADER_LENGTH 5

// longest message observed is 29 = 5 (header) + 22 (payload) + 2 (crc)
#define MAX_FRAME_SIZE 29


namespace aquamqtt{

class Frame;

class Frame final
{
private:
    FrameChannel mChannel;
    uint8_t buffer[MAX_FRAME_SIZE]; // the full message, including CRC
    uint8_t buffer_size;
    bool is_modified = false;

public:
    inline FrameChannel getChannel() const { return mChannel; };

    // a friendly name for the serial interface (listener, hmi, main)
    const char* getChannelName() const;

    const uint8_t* payload() const;
    int payload_size() const;

    // the header as a single int
    uint64_t getHeaderValue() const;

    inline const uint8_t* get_buffer() const { return buffer; };
    inline int get_buffer_size() const { return buffer_size; };

    bool isModified() const { return is_modified; };

    void replace_payload(uint8_t* payload);

    const std::string getBufferAsString() const;

    Frame& operator=(const Frame& other);
    bool operator==(const Frame& other) const;
    bool operator!=(const Frame& other) const;

public:
    Frame(const FrameChannel channel, const uint8_t* buffer, const uint8_t buffer_size);
    Frame(const Frame& other);
    ~Frame() = default;
};

}
#endif //AQUAMQTT_FRAME_H
