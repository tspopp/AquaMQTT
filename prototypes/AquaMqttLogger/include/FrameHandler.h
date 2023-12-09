#ifndef AQUAMQTT_FRAMEHANDLER_H
#define AQUAMQTT_FRAMEHANDLER_H

#include <Arduino.h>

#include "ESPLink.h"

class FrameHandler
{
public:
    FrameHandler(ESPLink* mqttClient);

    ~FrameHandler() = default;

    void handleFrame(uint8_t frameId, uint8_t payloadLength, uint8_t* payload);

private:
    void parse193(uint8_t payloadLength, const uint8_t* payload);
    void parse194(uint8_t payloadLength, const uint8_t* payload);
    void parse67(uint8_t payloadLength, const uint8_t* payload);

    void logDebug(uint8_t frameId, uint8_t payloadLength, uint8_t* payload);

    ESPLink* mMqttClient;

};

#endif  // AQUAMQTT_FRAMEHANDLER_H
