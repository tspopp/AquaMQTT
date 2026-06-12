#include "V5ListenerTask.h"

#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "util.h"
#include "mqtttask.h"
#include "protocol.h"


#define WRITE_QUEUE_SIZE 8

namespace aquamqtt
{

/*** NON-MEMBER FUNCTIONS ***/

void frameReceived(Frame &frame) {
    /*
    LOG.print("[framebuffer] processing message: header=");
    LOG.print(message.getHeaderValue());
    LOG.print("; payload_size=");
    LOG.print(message.payload_size());
    LOG.println();
    */

    // send frame to MQTT for debugging
    #ifdef MQTT_PUBLISH_FRAMES
    MqttTaskV5::getInstance().queueFrame(frame);
    #endif

    process_frame(frame);
}

void frameDropped(const uint8_t* buffer, uint8_t size) {
    // send frame to MQTT for debugging
    #ifdef MQTT_PUBLISH_FRAMES
    MqttTaskV5::getInstance().queueDroppedBytes(Frame(FrameChannel::CH_LISTENER, buffer, size));
    #endif
}

/*** FRAMEBUFFER PUBLIC ***/

FrameBuffer::FrameBuffer()
{
}

void FrameBuffer::pushByte(const uint8_t val)
{
    // drop old bytes if the buffer is full
    if (buffer_size >= MAX_FRAME_SIZE) {
        frameDropped(buffer, buffer_size);
        n_err_buffer_full++;
        buffer_size = 0;
    }

    // append the new element to the buffer
    buffer[buffer_size++] = val;

    // see if the buffer contains a full frame
    Frame* message = handleFrame();
    if (message) {
        frameReceived(*message);
        delete message;
    }
}

/*** FRAMEBUFFER PRIVATE ***/

Frame * FrameBuffer::handleFrame()
{
    if (dropped_size > MAX_FRAME_SIZE - 2) {
        frameDropped(dropped, dropped_size);
        dropped_size = 0;
    }

    // first byte must be 0x01
    if (buffer_size == 1 && buffer[0] != 0x01) {
        dropped[dropped_size++] = buffer[0];
        //LOG.print("dropping first byte: ");
        //LOG.println(buffer[0]);
        n_err_no_magic++;
        buffer_size = 0;
        return nullptr;
    }

    // second byte must be 0x64 or 0x65
    if (buffer_size == 2 && buffer[1] != 0x64 && buffer[1] != 0x65) {
        dropped[dropped_size++] = buffer[0];
        dropped[dropped_size++] = buffer[1];
        //LOG.print("dropping first 2 bytes: ");
        //LOG.print(buffer[0]);
        //LOG.print("-");
        //LOG.println(buffer[1]);
        n_err_no_magic++;
        buffer_size = 0;
        return nullptr;
    }

    if (dropped_size > 0) {
        frameDropped(dropped, dropped_size);
        dropped_size = 0;
    }

    if (buffer_size < HEADER_LENGTH + 2) {
        return nullptr;
    }

    // check if we have a message without a payload
    if (buffer_size == HEADER_LENGTH + 2) {
        if (check_crc(buffer, buffer_size)) {
            // CRC match -> we have a message without a payload
            n_frames_received++;
            Frame* msg = new Frame(FrameChannel::CH_LISTENER, buffer, buffer_size);
            buffer_size = 0;
            return msg;
        }
    }

    int payloadLength = buffer[HEADER_LENGTH];
    int messageLength = HEADER_LENGTH + 1 + payloadLength;

    if (messageLength + 2 > MAX_FRAME_SIZE) {
        LOG.println("[framebuffer] message has invalid length");
        n_err_frame_too_long++;
        frameDropped(buffer, buffer_size);
        buffer_size = 0;
        return nullptr;
    }

    // wait until buffer holds a complete frame
    if (buffer_size < messageLength + 2) {
        return nullptr;
    }

    if (!check_crc(buffer, buffer_size)) {
        LOG.println("[framebuffer] invalid CRC");
        n_err_checksum++;
        frameDropped(buffer, buffer_size);
        buffer_size = 0;
        return nullptr;
    }

    // completed and valid frame
    n_frames_received++;
    Frame* msg = new Frame(FrameChannel::CH_LISTENER, buffer, buffer_size);
    buffer_size = 0;
    return msg;
}


/*** SERIALTASK PUBLIC ***/

V5ListenerTask& V5ListenerTask::getInstance() {
    static V5ListenerTask instance(&Serial2, config::GPIO_MAIN_RX, 0, 0);
    return instance;
}

void V5ListenerTask::setup()
{
    Task::setup();

    log_line(taskName);
    _port->begin(38400, SERIAL_8N1, _gpio_rx, _gpio_tx);
    if (_gpio_enable_tx) {
        pinMode(_gpio_enable_tx, OUTPUT);
    }
}

void V5ListenerTask::loop()
{
    Task::loop();

    while (_port->available())
    {
        uint8_t value = _port->read();
        buffer.pushByte(value);
    }
}

/*** SERIALTASK PROTECTED ***/

void V5ListenerTask::periodicUpdate()
{
    Task::periodicUpdate();

    LOG.print("[");
    LOG.print(taskName);
    LOG.print("] messages received: ");
    LOG.print(buffer.countFramesReceived());
    LOG.print("; checksums errors: ");
    LOG.print(buffer.countChecksumErrors());
    LOG.print("; long messages: ");
    LOG.print(buffer.countLongFrameErrors());
    LOG.print("; buffer full: ");
    LOG.print(buffer.countBufferFullErrors());
    LOG.print("; no magic: ");
    LOG.print(buffer.countNoMagicErrors());
    LOG.println();
}

/*** SERIALTASK PRIVATE ***/

V5ListenerTask::V5ListenerTask(HardwareSerial *port, const uint8_t gpio_rx, const uint8_t gpio_tx, const uint8_t gpio_enable_tx)
    : Task("listener")
    , _port((HardwareSerial*)port)
    , _gpio_rx(gpio_rx)
    , _gpio_tx(gpio_tx)
    , _gpio_enable_tx(gpio_enable_tx)
{
}

}  // namespace aquamqtt
