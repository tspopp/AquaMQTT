# AquaMQTT (Work in Progress)

[![PlatformIO CI](https://github.com/tspopp/AquaMQTT/actions/workflows/main.yml/badge.svg)](https://github.com/tspopp/AquaMQTT/actions/workflows/main.yml)

Monitor and control your DHW heat pump locally using MQTT:

- Access sensor data and operational states from your heat pump.
- Control the heat pump: Set operational states e.g. water temperature and operation modes (e.g. BOOST).
- Automate the behaviour of your heat pump according to your local energy production or energy availability.
- Easily integrate AquaMQTT with smart home systems like [Home Assistant](https://www.home-assistant.io/).

<img src="../media/homeassistant.png?raw=true" width=40% height=40%>

## Compatible Devices

This project is built and tested against
the [Windhager AquaWin Air3](https://www.windhager.com/en/products/hot-water-tanks/aquawin-air3/) DHW heat pump.

It seems the heatpump is built by the [Groupe Atlantic](https://www.groupe-atlantic.fr/) and branded for various
companies. It is very likely but yet unconfirmed that this project is compatible to similar products with other branding
such as:

- [Atlantic Explorer](https://www.atlantic-comfort.com/Water-Heaters/Heat-pump-water-heaters/Explorer)
- [Austria Email Explorer](https://www.austria-email.de/produkte/waermepumpensysteme/waermepumpen-trinkwasser/explorer-evo-2/)
- [Thermor Aeromax](https://www.thermor.com/our-solutions/renewable-energy-solutions/heat-pump-water-heaters/aeromax-premium)
- [Panasonic DHW270](https://www.aircon.panasonic.eu/CH_de/model/paw-dhw270f/)

## How does it work?

This project intercepts the communication between the HMI controller and the main controller of the heatpump. Any
identified values contained within the serial messages are extracted, parsed and published via [MQTT](./MQTT.md). The
identified portions of the heatpump serial protocol is described in [PROTOCOL.md](./PROTOCOL.md). To intercept the
communication, an arduino based microcontroller is added between the communication lines of the HMI controller and the
main controller.

If AquaMQTT is used for controlling the heat pump, it will act as HMI controller and communicates directly with the main
controller of the heatpump, without the need for the original HMI controller.

## Why not using cozytouch / io-homecontrol?

Unfortunately, I had no success pairing those solutions with my Windhager branded heatpump.

## Roadmap

- [x] HW prototype: arduino nano + esp8266
- [x] SW prototype: Provide sensor data via MQTT
- [x] Visualize data within HomeAssistant
- [x] Prototype: Control the heat pump (replaces original HMI controller)
- [ ] Prototype: Control the heat pump (man in the middle)
- [ ] Final hardware, board layout and instructions
- [ ] HomeAssistant configuration


