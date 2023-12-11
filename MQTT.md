# MQTT


## Overall

The mqtt prefix is configurable within `AquaMQTT/include/config/ExampleConfiguration.h`: 

```c++
constexpr char     mqttPrefix[]     = "prefix/";
``````

Using the prefix, the `$root` topic is created, which is `$prefix/aquamqtt/` and used by all of the below messages.

## Publish Topics

### AquaMQTT / Statistics

| Value                            |                            MQTT Topic | Format | Unit |                                Other Information |
|----------------------------------|---------------------------------------|--------|------|--------------------------------------------------|
| AquaMQTT Last Will               | `$root/stats/lwlState`                |   Enum |      |                                ONLINE, OFFLINE   |
| AquaMQTT OperationMode           | `$root/stats/aquamqttMode`            |   Enum |      |                                 LISTENER, MITM   |
| AquaMQTT IP Address              | `$root/stats/ipAddress`               | string |      |                            e.g. 192.168.188.62   |
| AquaMQTT RSSI                    | `$root/stats/rssiDb`                  |    int |   dB |                                                  |
| AquaMQTT Messages OK             | `$root/stats/$channel/msgHandled`     | uint64 |      |                                                  |
| AquaMQTT Messages IGNORED        | `$root/stats/$channel/msgUnhandled`   | uint64 |      |                                                  |
| AquaMQTT Messages CRC NOK        | `$root/stats/$channel/msgCRCNOK`      | uint64 |      |                                                  |
| AquaMQTT Dropped Bytes           | `$root/stats/$channel/droppedBytes`   | uint64 |      |                                                  |
| AquaMQTT Active Overrides        | `$root/stats/$channel/activeOverrides` | ListOfEnum | |    e.g. "["operationMode", "waterTempTarget"]"   |

`$channel` is either `hmi` and `main` or `listener` depending on the AquaMQTT operation mode.


### HMI Message

| Value                            |                           MQTT Topic | Format | Unit |                                Other Information |
|----------------------------------|--------------------------------------|--------|------|--------------------------------------------------|
| Target Water Temperature         | `$root/hmi/waterTempTarget`          | float  |   °C |                                                  |
| Operation Mode                   | `$root/hmi/operationMode`            |   Enum |      |  AUTO, MAN ECO ON, MAN ECO OFF, BOOST, ABSENCE   |
| Operation Type                   | `$root/hmi/operationType`            |   Enum |      |                         TIME WINDOW, ALWAYS ON   |
| Operation Time Window A          | `$root/hmi/timerWindowA`             | string |      |                               e.g. 22:00-06:00   |
| Operation Time Window B          | `$root/hmi/timerWindowB`             | string |      |                               e.g. 04:00-08:00   |
| Current Time                     | `$root/hmi/time`                     | string |      |                                   e.g 09:11:59   |
| Current Date                     | `$root/hmi/date`                     | string |      |                                 e.g 07.09.2023   |
| Emergeny Mode Enabled            | `$root/hmi/emergencyMode`            |   bool |      |                                                  |
| Heating Element Enabled          | `$root/hmi/heatingElementEnabled`    |   bool |      |                                                  |
| Anti-Legionella Mode             | `$root/hmi/antiLegionellaPerMonth`   |  uint8 |      |                0 (off), 1 (once per month), etc  |
| Configuration AirDuct            | `$root/hmi/configAirduct`            |   Enum |      |                       INT/INT, INT/EXT, EXT/EXT  |
| Configuration Installation       | `$root/hmi/configInstallation`       |   Enum |      |                                                  |
| HMI SetupState                   | `$root/hmi/setupState`               |   Enum |      |                OK, PARTIAL RESET, FACTORY RESET  |


### Main Message

| Value                            |                          MQTT Topic | Format | Unit |                                Other Information |
|----------------------------------|-------------------------------------|--------|------|--------------------------------------------------|
| Current Water Temperature        | `$root/main/waterTemp`              | float  |   °C |                                                  |
| Supply Air Temperature           | `$root/main/supplyAirTemp`          | float  |   °C |                                                  |
| Lower Evaporator Air Temperature | `$root/main/evaporatorAirTempLower` | float  |   °C |                                                  |
| Upper Evaporator Air Temperature | `$root/main/evaporatorAirTemp`      | float  |   °C |                                                  |
| Fan Speed                        | `$root/main/fanSpeed`               | uint16 |  rpm |                                                  |
| State: Fan On/Off                | `$root/main/stateFan`               | bool   |      |                                                  |
| State: Heatpump On/Off           | `$root/main/stateHeatpump`          | bool   |      |                                                  |
| State: Heating Element On/Off    | `$root/main/stateElement`           | bool   |      |                                                  |
| State: Boiler Backup On/Off      | `$root/main/stateExtBoiler`         | bool   |      |                                                  |
| State: Defrost On/Off Date       | `$root/main/stateDefrost`           | bool   |      |                                                  |
     
### Energy Message


| Value                            |                            MQTT Topic | Format | Unit |                                Other Information |
|----------------------------------|---------------------------------------|--------|------|--------------------------------------------------|
| Total Heatpump Hours             | `$root/energy/totalHeatpumpHours`     | uint32 |    h |                                                  |
| Total Heating Element Hours      | `$root/energy/totalHeatingElemHours`  | uint32 |    h |                                                  |
| Total Hours                      | `$root/energy/totalHours`             | uint32 |    h |                                                  |
| Total Energy                     | `$root/energy/totalEnergyWh`          | uint64 |   Wh |                                                  |
| Current Power Heatpump           | `$root/energy/powerHeatpump`          | uint16 |    W |                                                  |
| Current Power Heating Element    | `$root/energy/powerHeatingElem`       | uint16 |    W |                                                  |
| Current Power Total              | `$root/energy/powerTotal`             | uint16 |    W |                                                  |

## Subscribe Topics

Using this topics you may override the HMI Controller in AquaMQTT OperationMode Man-In-The-Middle. Currently these are the only ones implemented:

| Value / Action                           |                       MQTT Topic | Format | Unit | Example Payload |                               Other Information |
|----------------------------------|----------------------------------|--------|------|-----------------|-------------------------------------------------|
| Target Water Temperature         | `$root/ctrl/waterTempTarget`     | float  |   °C |        "`55.0`" | Overrides the water temperature target: Allowed range is from 20°C to 62°C. |
| Operation Mode                   | `$root/ctrl/operationMode`       |   Enum |      |       "`BOOST`" | Overrides the operation mode, may affect the waterTempTarget. For example BOOST and ABSENCE will automatically set the waterTempTarget accordingly.  |
| Reset Overrides                  | `$root/ctrl/reset`       |   Void |      |        | Removes all previous set overrides. |


**Note:** Calling a `ctrl` topic with an empty payload `""` will reset individual override.