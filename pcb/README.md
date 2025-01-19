# AquaMqtt PCB Board

The AquaMQTT PCB Board is intended to be installed between the communication wires of the DHW HMI Controller and the DHW
Mainboard. To enable the communication to both HMI Controller and DHW Mainboard, the board provides two
individual [USART entities in One-Wire Mode](https://ww1.microchip.com/downloads/en/AppNotes/USART-in-One-Wire-Mode-ApplicationNote-DS00002658.pdf).
The Arduino ESP32 on the board runs AquaMQTT, providing communication via WiFi/MQTT while being powerful enough to
handle any time critical USART communication with the heat pumps hmi controller and main board.

<img src="./../img/board_front.png?raw=true" width=40% height=40%>

<img src="./../img/board_back.png?raw=true" width=40% height=40%>

Additionally, an optional DS3231 Real Time Clock might be added to the board. This might be handy in case AquaMQTT will be ever used without any HMI at all. It would ensure a recovery with correct date and time after power outage and without WiFi.
**Adding a RTC Module is experimental and untested!**

The board is designed using [Kicad](https://www.kicad.org/). You may need to install an
additional [Arduino Kicad library](https://github.com/Alarm-Siren/arduino-kicad-library) before opening/editing the
project.

## RAST vs. 2x4 Connector

Your [heat pump](./../DEVICES.md) may use a different connector such as the Thermor Aeromax 5. Happily [@tukutt](https://github.com/tukutt) designed an [adapter pcb](https://oshwlab.com/tukutt/aquamqtt-original-pcb-to-rast-co) to connect those heatpumps without cutting any cables. Read more about this in [#51](https://github.com/tspopp/AquaMQTT/issues/51).

## Changelog

| Version |       Description |
|---------|-------------------|
| 1.0     |    Initial Design |


## BOM


| Item                                                                                                                                                             | Count | Estimated Cost |
|------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------|----------------|
| AquaMqtt PCB Board (e.g. via AISLER.net)                                                                                                                                               | 1x   | ~ 7 €         |
| [Arduino Nano ESP32](https://docs.arduino.cc/hardware/nano-esp32). Note: Community suggested [cheaper alternative](https://github.com/tspopp/AquaMQTT/issues/56)                                                                                             | 1x    | ~ 20 €         |
| Cable 2x4 2,54mm (Socket-Housing, Contacts, Wiring)                                                                                                              | 1x    | ~ 10 €         |
| [Real Time Clock RTC DS3231](https://www.az-delivery.de/products/ds3231-real-time-clock) *(optional, experimental)*                                                            | (1x)  | ~ 6 €          |
| [SparkFun Logic Level Converter](https://eckstein-shop.de/SparkFunLogicLevelConverter-Bi-DirectionaShifterPegelwandlerEN) - 3.3V to 5V Bi-Directional or similar | 1x    | ~ 4 €          |
| 100k Resistance 0207 (Ø x L) 2.5 mm x 6.8 mm                                                                                                                     | 3x    | < 1 €          |
| 10k Resistance 0207 (Ø x L) 2.5 mm x 6.8 mm                                                                                                                      | 3x    | < 1 €          |
| 4.7k Resistance 0207 (Ø x L) 2.5 mm x 6.8 mm                                                                                                                     | 3x    | < 1 €          |
| [Transistor BC547A TO-92](https://www.conrad.de/de/p/tru-components-transistor-bjt-diskret-tc-bc547a-to-92-anzahl-kanaele-1-npn-1581825.html)                    | 4x    | < 1 €          |
| Pin Header 2x4 2,54mm                                                                                                                                           | 2x    | < 1 €          |
| [Pin Header 1x4 2,54mm](https://www.conrad.de/de/p/mpe-garry-stiftleiste-standard-anzahl-reihen-1-polzahl-je-reihe-4-087-1-004-0-s-xs0-1260-1-st-733940.html)   | 2x    | < 1 €          |
| [Pin Header 1x2 2,54mm](https://www.conrad.de/de/p/mpe-garry-stiftleiste-standard-anzahl-reihen-1-polzahl-je-reihe-2-087-1-002-0-s-xs0-1260-1-st-733900.html)   | 1x    | < 1 €          |



