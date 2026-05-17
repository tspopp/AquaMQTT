#ifndef AQUAMQTT_MODBUS_LISTENER_TASK_H
#define AQUAMQTT_MODBUS_LISTENER_TASK_H

#include <Arduino.h>

#include "message/optitronic2/ModbusRtuFrame.h"
#include "message/optitronic2/RegisterMap.h"

namespace aquamqtt
{

/**
 * Task that listens on the Modbus RTU bus (passive sniff) and parses
 * request/response pairs from the Optitronic 2 protocol.
 *
 * On the half-duplex bus, we see interleaved traffic:
 *   Master (HMI) -> Read Request (fn=0x03)
 *   Slave (Main) -> Read Response (fn=0x03)
 *   Master (HMI) -> Write Single (fn=0x06)
 *   Slave (Main) -> Write Echo (fn=0x06)
 *   etc.
 *
 * Frame boundaries are detected by inter-character silence (>2ms at 57600 baud).
 */
class ModbusListenerTask
{
public:
    ModbusListenerTask();

    void spawn();

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    void setup();
    void loop();
    void processFrame();

    // Frame assembly buffer
    uint8_t       mFrameBuffer[message::optitronic2::MODBUS_MAX_FRAME_SIZE];
    uint8_t       mFrameLength;
    unsigned long mLastByteTime;
    bool          mFrameInProgress;

    // Request tracking: we need to know what the last read request was
    // so we can map the response data back to register addresses
    bool     mPendingReadRequest;
    uint16_t mPendingReadStartReg;
    uint16_t mPendingReadQuantity;

    // Statistics
    uint32_t mFramesReceived;
    uint32_t mCrcErrors;
    uint32_t mRawBytesReceived;
    uint32_t mRawBytesHmi;
    uint32_t mLastStatsUpdate;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_MODBUS_LISTENER_TASK_H
