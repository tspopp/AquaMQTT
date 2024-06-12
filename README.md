# AquaMQTT

[![PlatformIO CI](https://github.com/tspopp/AquaMQTT/actions/workflows/main.yml/badge.svg)](https://github.com/tspopp/AquaMQTT/actions/workflows/main.yml)

AquaMQTT is a project designed to monitor and control your DHW heat pump locally using MQTT.

## Features

- Access sensor data and operational states from your heat pump.
- Control the heat pump: Set operational states e.g. water temperature and operation modes.
- [Automate the behavior of your heat pump according to your local energy production or energy availability](/README-PV.md)
- Easily integrate AquaMQTT with smart home systems like [Home Assistant](https://www.home-assistant.io/).

<p float="left">
   <img src="img/homeassistant.png?raw=true" width=35% height=35%>
   <img src="img/aquamqtt.jpg?raw=true" width=60% height=60%>
</p>


## Compatible Devices

AquaMQTT has been designed and tested with the [Windhager AquaWin Air3](https://www.windhager.com/en/products/hot-water-tanks/aquawin-air3/) DHW heat pump. While specifically tailored to this model, it's highly likely that AquaMQTT is compatible with similar heat pumps manufactured by the [Groupe Atlantic](https://www.groupe-atlantic.fr/) and branded for various companies. 

### Proofen compatible devices:

- [Atlantic Explorer V4](https://github.com/tspopp/AquaMQTT/issues/14)
- [Thermor Aeromax 5](https://github.com/tspopp/AquaMQTT/issues/18)
- [Windhager AquaWin Air3](https://www.windhager.com/en/products/hot-water-tanks/aquawin-air3/) 

### Other potential compatible devices include, but are not limited to:

- [Atlantic Explorer](https://www.atlantic-comfort.com/Water-Heaters/Heat-pump-water-heaters/Explorer)
- [Austria Email Explorer](https://www.austria-email.de/produkte/waermepumpensysteme/waermepumpen-trinkwasser/explorer-evo-2/)
- [Panasonic DHW270](https://www.aircon.panasonic.eu/CH_de/model/paw-dhw270f/)
- [Sauter Confort Pagosa Green](https://www.confort-sauter.com/chauffe-eau/pagosa-green)
- [Thermor Aeromax](https://www.thermor.com/our-solutions/renewable-energy-solutions/heat-pump-water-heaters/aeromax-premium)


If you have a heat pump with similar characteristics or branding, AquaMQTT might be compatible. Feel free to explore and adapt the project for your specific device, and share your experience.

## How does it work?

AquaMQTT facilitates communication between the HMI controller and the main controller of your heat pump. The project intercepts the serial messages exchanged between these controllers, extracts and parses identified values, and publishes them via [MQTT](./MQTT.md). For a detailed description of the heat pump's serial protocol, refer to [PROTOCOL.md](./PROTOCOL.md).

To achieve this communication interception, an Arduino-based microcontroller is introduced between the communication lines of the HMI controller and the main controller.

AquaMQTT not only monitors the heat pump's status but also provides the capability to control it by modifying the messages originating from the HMI controller. This allows you to set operational states, such as water temperature and operation modes, directly through AquaMQTT.

## Getting Started

- [Hardware Requirements](/pcb) 

- [Software Installation](AquaMQTT/README.md)

- [Heatpump Modification](./WIRING.md)


## Why not using cozytouch / io-homecontrol?

Unfortunately, I had no success pairing those solutions with my Windhager branded heatpump. Moreover, this solution is independent from any manufacturer backends and therefore running entirely within your private local area network. 

## Further Reading

- [Heat pump serial protocol](./PROTOCOL.md)

- [Available MQTT Topics and Payloads](./MQTT.md)

- [Utilize excess local energy using PV modes](./README-PV.md)

## Some references to AquaMQTT

- 🇫🇷 https://forum.hacf.fr/t/integration-dun-chauffe-eau-atlantic-explorer-en-mqtt/40493/6
- 🇳🇱 https://gathering.tweakers.net/forum/list_messages/2116816/78

