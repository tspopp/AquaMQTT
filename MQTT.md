# MQTT

TODO: Update this

| Value                            |                       MQTT Topic | Format | Unit |                                Other Information |
|----------------------------------|----------------------------------|--------|------|--------------------------------------------------|
| Target Water Temperature         | `%prefix/waterTempTarget`        | float  |   °C |                                                  |
| Current Water Temperature        | `%prefix/waterTemp`              | float  |   °C |                                                  |
| Operation Mode                   | `%prefix/operationMode`          | Enum   |      |  AUTO, MAN ECO ON, MAN ECO OFF, BOOST, ABSENCE   |
| Operation Type                   | `%prefix/operationType`          | Enum   |      |                         TIME WINDOW, ALWAYS ON   |
| Total Heatpump Hours             | `%prefix/totalHeatpumpHours`     | uint32 |    h |                                                  |
| Total Heating Element Hours      | `%prefix/totalHeatingElemHours`  | uint32 |    h |                                                  |
| Total Hours                      | `%prefix/totalHours`             | uint32 |    h |                                                  |
| Total Energy                     | `%prefix/totalEnergyWh`          | uint64 |   Wh |                                                  |
| Current Power Heatpump           | `%prefix/powerHeatpump`          | uint16 |    W |                                                  |
| Current Power Heating Element    | `%prefix/powerHeatingElem`       | uint16 |    W |                                                  |
| Current Power Total              | `%prefix/powerTotal`             | uint16 |    W |                                                  |
| Supply Air Temperature           | `%prefix/supplyAirTemp`          | float  |   °C |                                                  |
| Lower Evaporator Air Temperature | `%prefix/evaporatorAirTempLower` | float  |   °C |                                                  |
| Upper Evaporator Air Temperature | `%prefix/evaporatorAirTemp`      | float  |   °C |                                                  |
| Fan Speed                        | `%prefix/fanSpeed`               | uint16 |  rpm |                                                  |
| Current Time                     | `%prefix/time`                   | string |      |                                 e.g 09:11:59     |
| Current Date                     | `%prefix/date`                   | string |      |                                 e.g 07.09.2023   |
| State: Fan On/Off                | `%prefix/stateFan`               | bool   |      |                                                  |
| State: Heatpump On/Off           | `%prefix/stateHeatpump`          | bool   |      |                                                  |
| State: Heating Element On/Off    | `%prefix/stateElement`           | bool   |      |                                                  |
| State: Boiler Backup On/Off      | `%prefix/stateExtBoiler`         | bool   |      |                                                  |
| State: Defrost On/Off Date       | `%prefix/stateDefrost`           | bool   |      |                                                  |
