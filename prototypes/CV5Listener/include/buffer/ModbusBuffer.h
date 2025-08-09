#ifndef MODBUSBUFFER_H
#define MODBUSBUFFER_H

#include <Arduino.h>
#include <FastCRC.h>
#include <MQTTClient.h>
#include <RingBuf.h>

#include "../config/Configuration.h"

namespace aquamqtt{

namespace modbus{

class ModbusBuffer
{
public:
  explicit ModbusBuffer();
    explicit ModbusBuffer(MQTTClient * mqtt_client);

    ~ModbusBuffer() = default;

  int pushByte(int val);

  uint64_t getDroppedCount() const;

  uint64_t getCRCFailedCount() const;

  uint64_t getUnhandledCount() const;

  uint64_t getHandledCount() const;


  private:

  MQTTClient* mMQTTClient;

  int handleFrame();

  bool isSync();

  static void     toHexStr(uint8_t* data, uint8_t data_len, char* buffer);

  RingBuf<int, config::MODBUS_MAX_LENGTH> mBuffer;
  uint8_t mTransferBuffer[config::MODBUS_MAX_LENGTH];

  FastCRC16 mCRC;

  uint64_t mDroppedCount;
  uint64_t mCRCFailCount;
  uint64_t mUnhandledCount;
  uint64_t mHandledCount;

  uint8_t                   mTopicBuffer[config::MQTT_MAX_TOPIC_SIZE];
  uint8_t                   mPayloadBuffer[config::MQTT_MAX_PAYLOAD_SIZE];

  uint8_t       mPreviousFrameId;
  unsigned long mLastValidFrameTimestamp;
};
}
}
#endif //MODBUSBUFFER_H
