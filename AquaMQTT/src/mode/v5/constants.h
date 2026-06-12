#ifndef AQUAMQTT_CONSTANTS_H
#define AQUAMQTT_CONSTANTS_H

#include "message/MessageConstants.h"
#include "config/Configuration.h"


namespace aquamqtt
{

#define MQTT_PUBLISH_FRAMES

#define LOG Serial

#define HA_ORIGIN_NAME "AquaMQTT"
#define HA_DEVICE_NAME "DHW"

#define FrameChannel message::FrameBufferChannel

}  // namespace aquamqtt

#endif  // AQUAMQTT_CONSTANTS_H
