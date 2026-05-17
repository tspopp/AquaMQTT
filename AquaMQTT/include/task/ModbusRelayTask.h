#ifndef AQUAMQTT_MODBUS_RELAY_TASK_H
#define AQUAMQTT_MODBUS_RELAY_TASK_H

#include <Arduino.h>

#include "message/optitronic2/ModbusRtuFrame.h"
#include "message/optitronic2/RegisterMap.h"

namespace aquamqtt
{

/**
 * Task that sits between HMI and Main Controller on the Modbus RTU bus (MITM).
 *
 * With the passthrough jumper removed, this task:
 *   1. Receives frames from HMI (Serial1) → forwards to Main (Serial2)
 *   2. Receives frames from Main (Serial2) → forwards to HMI (Serial1)
 *   3. Parses all traffic for state extraction (same as listener)
 *   4. Can inject modified write frames when MQTT commands are pending
 *
 * Both serial lines are half-duplex at 57600 8N1.
 * Frame boundaries are detected by inter-character silence (>2ms).
 */
class ModbusRelayTask
{
public:
    ModbusRelayTask();

    void spawn();
    void setup();
    void loop();

private:
    [[noreturn]] static void innerTask(void* pvParameters);

    // Process a complete frame received from one side and forward to the other
    void processAndForward(uint8_t* buffer, uint8_t length, HardwareSerial& destination, bool fromHmi);
    void parseFrame(uint8_t* buffer, uint8_t length, bool fromHmi);
    void injectPendingWrites();
    void injectPeriodicReads();
    void sendToMain(uint8_t* frame, uint8_t len);
    int  receiveFromMain(uint8_t* buf, uint8_t maxLen, uint16_t timeoutMs = 100);

    // HMI side (Serial1) frame assembly
    uint8_t       mHmiFrameBuffer[message::optitronic2::MODBUS_MAX_FRAME_SIZE];
    uint8_t       mHmiFrameLength;
    unsigned long mHmiLastByteTime;
    bool          mHmiFrameInProgress;

    // Main controller side (Serial2) frame assembly
    uint8_t       mMainFrameBuffer[message::optitronic2::MODBUS_MAX_FRAME_SIZE];
    uint8_t       mMainFrameLength;
    unsigned long mMainLastByteTime;
    bool          mMainFrameInProgress;

    // Request tracking for response mapping
    bool     mPendingReadRequest;
    uint16_t mPendingReadStartReg;
    uint16_t mPendingReadQuantity;

    // Statistics
    uint32_t mFramesReceived;
    uint32_t mCrcErrors;
    uint32_t mFramesRelayed;
    uint32_t mWritesInjected;
    uint32_t mHmiFramesIn;
    uint32_t mMainFramesIn;
    uint32_t mHmiBytesIn;
    uint32_t mMainBytesIn;
    uint32_t mLastStatsUpdate;
    uint32_t mEchoBytes;
    uint32_t mTxBytesWritten;

    // Periodic read injection
    unsigned long mLastPeriodicRead;
    uint8_t       mPeriodicReadIndex;

    // Last forwarded frame for debugging
    uint8_t  mLastFwdFrame[32];
    uint8_t  mLastFwdFrameLen;

public:
    uint32_t getFramesReceived() const { return mFramesReceived; }
    uint32_t getCrcErrors() const { return mCrcErrors; }
    uint32_t getFramesRelayed() const { return mFramesRelayed; }
    uint32_t getHmiFramesIn() const { return mHmiFramesIn; }
    uint32_t getMainFramesIn() const { return mMainFramesIn; }
    uint32_t getHmiBytesIn() const { return mHmiBytesIn; }
    uint32_t getMainBytesIn() const { return mMainBytesIn; }
    uint32_t getEchoBytes() const { return mEchoBytes; }
    uint32_t getTxBytesWritten() const { return mTxBytesWritten; }
    uint32_t getWritesInjected() const { return mWritesInjected; }
    const uint8_t* getLastFwdFrame() const { return mLastFwdFrame; }
    uint8_t getLastFwdFrameLen() const { return mLastFwdFrameLen; }
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_MODBUS_RELAY_TASK_H
