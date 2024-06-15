# AquaDebug

AquaDebug is a simple serial bus debugging application meant to be flashed on the AquaMQTT Board for troubleshooting the
heat-pumps serial protocol.

- AquaDebug is sniffing the serial protocol without altering or interpreting any data
- AquaDebug expects the AquaMQTT Board to be configured as LISTENER
- AquaDebug reads the heat-pumps serial protocol and provides raw data to MQTT
- A python script running on any machine collects the data from MQTT and writes the data into files

In case you suspect that your heat-pump is incompatible with the existing AquaMQTT implementation, due to a different
protocol, CRC or any other reason, you may try AquaDebug and provide a trace from your heat-pump communication as an
github issue.

## Getting Started

Refer to the [AquaMQTT README](./../../AquaMQTT/README.md) for installation and configuration instructions.

## Debugging / Troubleshooting

As soon as AquaDebug has been flashed, it will publish mqtt messages to `$prefix/aquadbg/debug` in case values have been
read from the Serial port.

These data is collected by a little python script and stored on your machine.

### Create the python environment

```
python3 -m venv venv
source venv/bin/activate
pip install paho-mqtt
```

### Run the python script

```
source venv/bin/activate
python3 collect.py
```