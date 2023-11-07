# AquaMQTTLogger

*Prototype*

Intercepts the one-wire bus, parsing messages exchanged by the HMI Controller and the main board.

Any messages known in the [Protocol description](../Protocol.md) are extracted and published to
several [MQTT topics](../MQTT.md).

The project runs on an Arduino Mega 2560 together with an ESP8266 piggyback (running esp-link) for WiFi/MQTT functionalities.

For proofing controllability of the heatpump, AquaMQTTLogger *may* act as simplified HMI controller via MQTT.
*The original HMI controller must be disconnected in this case.*

**Successfully tested against the original main controller**