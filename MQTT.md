# MQTT

| Value                            |                       MQTT Topic | Format | Unit | Other Information |
|----------------------------------|----------------------------------|--------|------|-------------------|
| Target Water Temperature         | `%prefix/waterTempTarget`        | float  |   °C |                   |
| Current Water Temperature        | `%prefix/waterTemp`              | float  |   °C |                   |
| Operation Mode                   | `%prefix/operationMode`          | string |      |               TBD |
| Operation Status                 | `%prefix/193bitmask`             | Enum   |      |               TBD |
| Total Heatpump Hours             | `%prefix/totalHeatpumpHours`     | uint32 |    h |                   |
| Total Heating Element Hours      | `%prefix/totalHeatingElemHours`  | uint32 |    h |                   |
| Total Hours                      | `%prefix/totalHours`             | uint32 |    h |                   |
| Total Energy                     | `%prefix/totalEnergyWh`          | uint64 |   Wh |                   |
| Current Power Heatpump           | `%prefix/powerHeatpump`          | uint16 |    W |                   |
| Current Power Heating Element    | `%prefix/powerHeatingElem`       | uint16 |    W |                   |
| Current Power Total              | `%prefix/powerTotal`             | uint16 |    W |                   |
| Supply Air Temperature           | `%prefix/supplyAirTemp`          | float  |   °C |                   |
| Lower Evaporator Air Temperature | `%prefix/evaporatorAirTempLower` | float  |   °C |                   |
| Upper Evaporator Air Temperature | `%prefix/evaporatorAirTemp`      | float  |   °C |                   |
| Fan Speed                        | `%prefix/fanSpeed`               | uint16 |  rpm |                   |
| Current Time                     | `%prefix/time`                   | string |      |  e.g 09:11:59     |
| Current Date                     | `%prefix/date`                   | string |      |  e.g 07.09.2023   |

## Debug Messages

| Value                     |         MQTT Topic |     Format |         Other Information |
|---------------------------|--------------------|------------|---------------------------|
| Payload of message 193    | `%prefix/dbg/193`  | hex string | Sent from main controller |
| Payload of message 67     | `%prefix/dbg/67`   | hex string | Sent from main controller |
| Payload of message 194    | `%prefix/dbg/194`  | hex string | Sent from HMI controller  |
