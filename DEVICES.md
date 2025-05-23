# Compatible Devices

## Proofen devices

These devices have been tested succesfully by the community. Currently there are no insights if the software version indicitates compatibilty to the AquaMQTT implementation.

| Name                                                                                               | HMI Software Version | Controller Software Version | Observed Protocol / Checksum         | Other Information                   | Installation Date |
|----------------------------------------------------------------------------------------------------|----------------------|-----------------------------|--------------------------------------|-------------------------------------|-------------------|
| [Atlantic Calypso Split Inverter 270l (BmdOnline)](https://github.com/tspopp/AquaMQTT/issues/64)                  | F                    | G                           | [LEGACY](PROTOCOL.md) / XOR          | [Rev 1.0 PCB: Adapter is recommended](./pcb) |                   |
| [Atlantic Calypso VR (Arrnooo, Quentin90000, sebcaps)](https://github.com/tspopp/AquaMQTT/issues/67)                  | E                    | F                           | [LEGACY](PROTOCOL.md)           | [Rev 2.0 PCB required](./pcb) |                   |
| [Atlantic Explorer 270/V3 (Silmo)](https://github.com/tspopp/AquaMQTT/pull/52)                     | E                    | E                           | [NEXT](PROTOCOL_NEXT.md) / XOR       |                                     |                   |
| [Atlantic Explorer (ALENOC)](https://github.com/tspopp/AquaMQTT/issues/14#issuecomment-2501636900) | E                    | E                           | [NEXT](PROTOCOL_NEXT.md) / XOR       |                                     |                   |
| [Atlantic Odyssee Pi (FSlunkk)](https://github.com/tspopp/AquaMQTT/issues/68)                      | B                    | B                           | [ODYSSEE](PROTOCOL_ODYSSEE.md) / XOR |                                     | 2015              |
| [Atlantic Explorer V4](https://github.com/tspopp/AquaMQTT/issues/14)                               | B                    | B                           | [LEGACY](PROTOCOL.md) / CRC16        |                                     |                   |
| [Austria Email Explorer Evo 2](https://github.com/tspopp/AquaMQTT/issues/22)                       | ?                    | ?                           | [LEGACY](PROTOCOL.md) / CRC16        |                                     |                   | 
| Austria Email Explorer Evo 2 260L + HX (Dominik)                                                   | B                    | B                           | [LEGACY](PROTOCOL.md)                |                                     | 2023                   |
| [Austria Email BWWP 200 WT SMART COZY (taloriko)](https://github.com/tspopp/AquaMQTT/issues/45)    | E                    | E                           | [NEXT](PROTOCOL_NEXT.md) / XOR       |                                     |               |
| [Thermor Aeromax 5](https://github.com/tspopp/AquaMQTT/issues/18)                                  | ?                    | ?                           | [LEGACY](PROTOCOL.md) / CRC16        | [Rev 1.0 PCB: Adapter is recommended](./pcb) |                   |
| [Thermor Aeromax 5](https://github.com/tspopp/AquaMQTT/issues/51)                                  | E                    | G                           | [LEGACY](PROTOCOL.md) / CRC16        | [Rev 1.0 PCB: Adapter is recommended](./pcb) |                   |
| Windhager Aquawin Air 3                                                                            | B                    | B                           | [LEGACY](PROTOCOL.md) / CRC16        |                                     | 2021              |


## Potential compatible devices

- [Atlantic Explorer](https://www.atlantic-comfort.com/Water-Heaters/Heat-pump-water-heaters/Explorer)
- [Austria Email Explorer](https://www.austria-email.de/produkte/waermepumpensysteme/waermepumpen-trinkwasser/explorer-evo-2/)
- [Panasonic DHW270](https://www.aircon.panasonic.eu/CH_de/model/paw-dhw270f/)
- [Sauter Confort Pagosa Green](https://www.confort-sauter.com/chauffe-eau/pagosa-green)
- [Thermor Aeromax](https://www.thermor.com/our-solutions/renewable-energy-solutions/heat-pump-water-heaters/aeromax-premium)
