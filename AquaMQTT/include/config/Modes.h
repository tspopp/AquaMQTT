#ifndef AQUAMQTT_MODES_H
#define AQUAMQTT_MODES_H

namespace aquamqtt::config
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

    /**
     * Optitronic 2 Modbus RTU Listener mode:
     * - Passively sniffs the Modbus RTU bus between HMI and main controller
     * - Parses read responses and write commands to extract register values
     * - Publishes decoded register data to MQTT with HA discovery
     * - Accepts MQTT commands and injects Modbus writes during bus idle periods
     * - Protocol: Standard Modbus RTU, 57600 8N1, slave 0x01
     * - Compatible with: Austria Email WPA 450 ECO and similar Optitronic 2 devices
     */
    OPTITRONIC2_LISTENER,

    /**
     * Optitronic 2 Modbus RTU MITM (Man-In-The-Middle) mode:
     * - Passthrough jumper REMOVED — ESP32 actively relays between HMI and Main
     * - Receives frames from HMI (Serial1) and forwards to Main (Serial2)
     * - Receives frames from Main (Serial2) and forwards to HMI (Serial1)
     * - Parses all traffic for state extraction and MQTT publishing
     * - Injects MQTT-commanded writes during bus idle periods
     * - Protocol: Standard Modbus RTU, 57600 8N1, slave 0x01
     */
    OPTITRONIC2_MITM,
};
}

#endif  // AQUAMQTT_MODES_H
