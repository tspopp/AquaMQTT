# Integrate AquaMQTT into HomeAssisant

## Automatic via MQTT Discovery

Since Release [v1.4.0](https://github.com/tspopp/AquaMQTT/releases/tag/v1.4.0) AquaMQTT supports the [MQTT Discovery](https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery) protocol from HomeAssistant. Therefore, a new AquaMQTT device will automatically appear in your homeassistant instance, as soon as the following pre-conditions are fullfilled:

- [x] AquaMQTT is connected to the MQTT broker of your HomeAssistant instance
- [x] The correct home assistant mqtt prefix has been set and the MQTT discovery has been set to enabled within [Configuration.h](./AquaMQTT/include/config/Configuration.h) prior to flashing AquaMQTT on the Arduino.
- [x] The first valid message from the heat pump has been parsed successfully. Reason: There are different heat pump protocols and as soon the protocol version is known, we know the set of attributes available.

If all these conditions are fullfilled, you will find the AquaMQTT device at `Settings` -> `Device` -> `MQTT/Devices` within your HomeAssistant web interface.

## Manual by editing your own configuration.yaml

If you need full control over the entities read from AquaMQTT, you may directly edit the `configuration.yaml` file of your homeassistant installation. There is a file [./aquamqtt.yaml](./aquamqtt.yaml) within this repository, which acts as template for you to edit and included in your configuration file. 

Please note, that there might be more attributes not implemented by this example. For a full catalogue of MQTT topics and payloads inspect [./MQTT.md](./MQTT.md) or inspect the MQTT traffic produced by AquaMQTT.