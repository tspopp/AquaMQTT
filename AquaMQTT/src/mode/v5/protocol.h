#ifndef AQUAMQTT_V5_PROTOCOL_H
#define AQUAMQTT_V5_PROTOCOL_H

#include "Frame.h"
#include "SerialRelayTask.h"


namespace aquamqtt{

bool check_crc(const uint8_t* frame, const uint8_t len);
uint16_t calculate_crc(const uint8_t* frame, const uint8_t len);

void process_frame(Frame &frame);


class V5Protocol : public ProtocolCallback
{
public:
    bool frameIsReady(message::FrameBufferChannel channel, uint8_t* buffer, uint8_t len) override;
    bool processFrame(message::FrameBufferChannel channel, uint8_t* frame_buffer, uint8_t frame_len, char* err_message_buffer, uint8_t max_err_message_len) override;
};


}
#endif // AQUAMQTT_V5_PROTOCOL_H
