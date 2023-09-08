#ifndef AQUAMQTT_FRAMEHANDLER_H
#define AQUAMQTT_FRAMEHANDLER_H

#include <Arduino.h>

#include "../.pio/libdeps/nanoatmega328new/ELClient/ELClient.h"
#include "../.pio/libdeps/nanoatmega328new/ELClient/ELClientCmd.h"
#include "../.pio/libdeps/nanoatmega328new/ELClient/ELClientMqtt.h"

class FrameHandler
{
public:
    FrameHandler();

    ~FrameHandler() = default;

    bool setup();

    void loop();

    void handleFrame(uint8_t frameId, uint8_t payloadLength, uint8_t* payload);

private:
    void parse193(uint8_t payloadLength, const uint8_t* payload);
    void parse194(uint8_t payloadLength, const uint8_t* payload);
    void parse67(uint8_t payloadLength, const uint8_t* payload);

    void logDebug(uint8_t frameId, uint8_t payloadLength, uint8_t* payload);

    ELClient     mElClient;
    ELClientCmd  mELClientCmd;
    ELClientMqtt mELClientMqtt;
};

#endif  // AQUAMQTT_FRAMEHANDLER_H
