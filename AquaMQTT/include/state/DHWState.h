#ifndef AQUAMQTT_DHWSTATE_H
#define AQUAMQTT_DHWSTATE_H

#include <Arduino.h>

#include "message/MessageConstants.h"

namespace aquamqtt
{

struct BufferStatistics
{
    uint64_t msgHandled;
    uint64_t msgUnhandled;
    uint64_t msgCRCFail;
    uint64_t droppedBytes;
    uint64_t msgSent;
};

/**
 * DHWState contains the original state originated by the original DHW controllers
 * FrameBuffers are writing complete frames to the DHWState storing the last known messages
 * In case a message has changed, it will trigger the mqtt task to publish updates to the broker.
 */
class DHWState
{
public:
    static DHWState& getInstance();

    virtual ~DHWState() = default;

    DHWState(const DHWState&) = delete;

private:
    DHWState();

public:
    DHWState& operator=(const DHWState&) = delete;

    message::ProtocolVersion getVersion();

    void setVersion(message::ProtocolVersion version);

    void setListener(TaskHandle_t handle);

    void storeFrame(uint8_t frameId, uint8_t payloadLength, uint8_t* payload);

    void updateFrameBufferStatistics(uint8_t source, BufferStatistics statistics);

    BufferStatistics getFrameBufferStatistics(uint8_t source);

    size_t copyFrame(uint8_t frameId, uint8_t* buffer, message::ProtocolVersion& version);

private:
    TaskHandle_t mNotify;

    SemaphoreHandle_t mMutex;

    message::ProtocolVersion mProtocolVersion;

    bool mHasHmiMessage;
    bool mHasMainMessage;
    bool mHasEnergyMessage;
    bool mHasErrorMessage;

    uint8_t mMessageHmi[aquamqtt::message::HEATPUMP_MAX_FRAME_LENGTH];
    uint8_t mMessageMain[aquamqtt::message::HEATPUMP_MAX_FRAME_LENGTH];
    uint8_t mMessageEnergy[aquamqtt::message::HEATPUMP_MAX_FRAME_LENGTH];
    uint8_t mMessageError[aquamqtt::message::HEATPUMP_MAX_FRAME_LENGTH];

    BufferStatistics mHmiStats;
    BufferStatistics mMainStats;
    BufferStatistics mListenerStats;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_DHWSTATE_H
