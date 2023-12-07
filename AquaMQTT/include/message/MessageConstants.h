#ifndef AQUAMQTT_MESSAGECONSTANTS_H
#define AQUAMQTT_MESSAGECONSTANTS_H

#include "Arduino.h"

namespace aquamqtt
{
namespace message
{
constexpr uint8_t HEATPUMP_MAX_FRAME_LENGTH = 40;

constexpr uint8_t HMI_MESSAGE_IDENTIFIER    = 194;
constexpr uint8_t HMI_MESSAGE_LENGTH        = 35;
constexpr uint8_t MAIN_MESSAGE_IDENTIFIER   = 193;
constexpr uint8_t MAIN_MESSAGE_LENGTH       = 37;
constexpr uint8_t ENERGY_MESSAGE_IDENTIFIER = 67;
constexpr uint8_t ENERGY_MESSAGE_LENGTH     = 31;

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_MESSAGECONSTANTS_H
