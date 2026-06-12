#ifndef AQUAMQTT_SERIAL_RELAY_TASK_H
#define AQUAMQTT_SERIAL_RELAY_TASK_H

#include <Arduino.h>

#include <vector>
#include <functional>

#include "message/MessageConstants.h"

#include "mode/v5/Task.h"


#define ERROR_MESSAGE_SIZE 200

namespace aquamqtt
{

class ProtocolCallback
{
public:
    /**
     * Handle a frame.
     * 
     * Arguments:
     * - channel: the channel where the frame was read
     * - buffer: where the frame contents are stored
     * - len: the length of the frame in bytes
     * - err_message: if there was an error, the error message will be stored here
     * - err_message_limit: the size of the error message buffer, in bytes
     * 
     * Return true if the frame was valid (and should be forwarded)
     */
    virtual bool frameIsReady(message::FrameBufferChannel channel, uint8_t* buffer, uint8_t len) = 0;

    /**
     * Return true iff the frame is ready to be processed. This means that it is either complete, or invalid and should be discarded.
     */
    virtual bool processFrame(message::FrameBufferChannel channel, uint8_t* frame_buffer, uint8_t frame_len, char* err_message_buffer, uint8_t max_err_message_len) = 0;
};


/**
 * Generic relay task that forwards messages between HMI and Main Controller on\
 * the serial interface (MITM).
 *
 * With the passthrough jumper removed, this task:
 *   1. Receives frames from HMI (Serial1) → forwards to Main (Serial2)
 *   2. Receives frames from Main (Serial2) → forwards to HMI (Serial1)
 *   3. Sends frames to callback function for state extraction and optional frame manipulation
 *
 * Both serial lines are half-duplex 8N1.
 * Frame boundaries are detected by inter-character silence (>4ms).
 */
class SerialRelayTask : public Task
{
public:
    SerialRelayTask(int baud_rate, int max_frame_size);
    virtual ~SerialRelayTask();

    virtual void setup() override;
    virtual void loop() override;

    void setCallback(ProtocolCallback* callback) { protocol_callback = callback; };

    static SerialRelayTask& getInstance();

protected:
    virtual void periodicUpdate() override;

private:
    void processAndForwardIfReady(message::FrameBufferChannel channel, uint8_t* buffer, uint8_t length, HardwareSerial& destination, long delay);

    // Process a complete frame received from one side and forward to the other
    void processAndForward(message::FrameBufferChannel channel, uint8_t* buffer, uint8_t length, HardwareSerial& destination);

    int baudRate;
    int maxFrameSize;
    ProtocolCallback* protocol_callback;

    // HMI side (Serial1) frame assembly
    uint8_t*      mHmiFrameBuffer;
    uint8_t       mHmiFrameLength = 0;
    unsigned long mHmiLastByteTime = 0;
    bool          mHmiFrameInProgress = false;

    // Main controller side (Serial2) frame assembly
    uint8_t*      mMainFrameBuffer;
    uint8_t       mMainFrameLength = 0;
    unsigned long mMainLastByteTime = 0;
    bool          mMainFrameInProgress = false;

    // Statistics
    uint32_t mFramesReceived = 0;
    uint32_t mFramesRelayed = 0;
    uint32_t mHmiFramesIn = 0;
    uint32_t mMainFramesIn = 0;
    uint32_t mHmiBytesIn = 0;
    uint32_t mMainBytesIn = 0;
    uint32_t mEchoBytes = 0;
    uint32_t mTxBytesWritten = 0;

    // buffer reserved for the last error message
    char err_message[ERROR_MESSAGE_SIZE];

public:
    uint32_t getFramesReceived() const { return mFramesReceived; }
    uint32_t getFramesRelayed() const { return mFramesRelayed; }
    uint32_t getHmiFramesIn() const { return mHmiFramesIn; }
    uint32_t getMainFramesIn() const { return mMainFramesIn; }
    uint32_t getHmiBytesIn() const { return mHmiBytesIn; }
    uint32_t getMainBytesIn() const { return mMainBytesIn; }
    uint32_t getEchoBytes() const { return mEchoBytes; }
    uint32_t getTxBytesWritten() const { return mTxBytesWritten; }
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_SERIAL_RELAY_TASK_H
