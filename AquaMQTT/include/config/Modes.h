#ifndef AQUAMQTT_MODES_H
#define AQUAMQTT_MODES_H

namespace aquamqtt
{
namespace config
{
enum EOperationMode
{
    /**
     *  AquaMqtt is acting as Listener and is connected to a single physical one-wire USART instance:
     * - Reads traffic from the HMI and MAIN Controller
     * - Parses and publishes DHW messages to MQTT
     */
    LISTENER,

    /**
     * AquaMqtt is acting as Man-In-The-Middle and is connected to two physical one-wire USART instances:
     * - Forwards data from the HMI Controller to the MAIN Controller
     * - Forwards data from the MAIN Controller to the HMI Controller
     * - Possibility to overwrite dedicated fields in the messages from HMI to Main Controller
     * - Parses and publishes DHW messages to MQTT, Allows modification via MQTT
     */
    MITM,
};
}
}  // namespace aquamqtt

#endif  // AQUAMQTT_MODES_H
