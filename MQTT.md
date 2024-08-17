# MQTT

## Overall

The mqtt prefix is configurable within `AquaMQTT/include/config/ExampleConfiguration.h`:

```c++
constexpr char     mqttPrefix[]     = "prefix/";
``````

Using the prefix, the `$root` topic is created, which is `$prefix/aquamqtt/` and used by all of the below messages.

## Publish Topics

### AquaMQTT / Statistics

| Value                   | MQTT Topic                          | Format | Unit | Other Information                                                                                                                                                                                                                                                    |
|-------------------------|-------------------------------------|--------|------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Last Will               | `$root/stats/lwlState`              | Enum   |      | ONLINE, OFFLINE  -- retained                                                                                                                                                                                                                                         |
| OperationMode           | `$root/stats/aquamqttMode`          | Enum   |      | LISTENER, MITM                                                                                                                                                                                                                                                       |
| Active Overrides (HMI)  | `$root/stats/activeOverrides`       | json   |      | Active Overrides are flagged either with 1 (overridden) or 0 (not overridden)  Example Payload:  `{ "operationMode": 0, "operationType": 0, "waterTempTarget": 0, "heatingElementEnabled": 0, "emergencyModeEnabled": 0, "configInstallation": 0 , "time/date": 1 }` |
| Active Overrides (MAIN) | `$root/stats/activeOverridesMain`   | json   |      | Active Overrides are flagged either with 1 (overridden) or 0 (not overridden)  Example Payload:  `{ "statePV": 1, "stateSolar": 1 }`                                                                                                                                                  |
| Override Modes          | `$root/stats/overrideMode`          | Enum   |      | `STANDARD`, `PV HP`, `PV HE` or `PV BOOST`. See [README-PV.md](/README-PV.md) for additional information.                                                                                                                                                            |
| Flag PV heat pump       | `$root/stats/flagPVModeHeatPump`    | bool   |      | Status of the pv heat pump flag. See [README-PV.md](/README-PV.md) for additional information.                                                                                                                                                                       |
| Flag PV heat element    | `$root/stats/flagPVModeHeatElement` | bool   |      | Status of the pv heat element flag. See [README-PV.md](/README-PV.md) for additional information.                                                                                                                                                                    |
| IP Address              | `$root/stats/ipAddress`             | string |      | e.g. 192.168.188.62                                                                                                                                                                                                                                                  |
| RSSI                    | `$root/stats/rssiDb`                | int    | dB   |                                                                                                                                                                                                                                                                      |
| Messages OK             | `$root/stats/$channel/msgHandled`   | uint64 |      |                                                                                                                                                                                                                                                                      |
| Messages IGNORED        | `$root/stats/$channel/msgUnhandled` | uint64 |      |                                                                                                                                                                                                                                                                      |
| Messages CRC NOK        | `$root/stats/$channel/msgCRCNOK`    | uint64 |      |                                                                                                                                                                                                                                                                      |
| Dropped Bytes           | `$root/stats/$channel/droppedBytes` | uint64 |      |                                                                                                                                                                                                                                                                      |

`$channel` is either `hmi` and `main` or `listener` depending on the AquaMQTT operation mode.

### HMI Message

| Value                         | MQTT Topic                         | Format | Unit | Other Information                                                                                                                                                                      |
|-------------------------------|------------------------------------|--------|------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Target Water Temperature      | `$root/hmi/waterTempTarget`        | float  | °C   |                                                                                                                                                                                        |
| Operation Mode                | `$root/hmi/operationMode`          | Enum   |      | AUTO, MAN ECO ON, MAN ECO OFF, BOOST, ABSENCE                                                                                                                                          |
| Operation Type                | `$root/hmi/operationType`          | Enum   |      | TIME WINDOW, ALWAYS ON                                                                                                                                                                 |
| Operation Time Window A       | `$root/hmi/timerWindowA`           | string |      | e.g. 22:00-06:00                                                                                                                                                                       |
| Operation Time Window B       | `$root/hmi/timerWindowB`           | string |      | e.g. 04:00-08:00                                                                                                                                                                       |
| Current Time                  | `$root/hmi/time`                   | string |      | e.g 09:11:59                                                                                                                                                                           |
| Current Date                  | `$root/hmi/date`                   | string |      | e.g 07.09.2023                                                                                                                                                                         |
| Emergency Mode Enabled        | `$root/hmi/emergencyMode`          | bool   |      |                                                                                                                                                                                        |
| Heating Element Enabled       | `$root/hmi/heatingElementEnabled`  | bool   |      |                                                                                                                                                                                        |
| Anti-Legionella Mode          | `$root/hmi/antiLegionellaPerMonth` | uint8  |      | 0 (off), 1 (once per month), etc                                                                                                                                                       |
| Configuration AirDuct         | `$root/hmi/configAirduct`          | Enum   |      | INT/INT, INT/EXT, EXT/EXT                                                                                                                                                              |
| Configuration Installation    | `$root/hmi/configInstallation`     | Enum   |      | HEAT PUMP ONLY, BOILER BACKUP / HEAT PUMP PRIORITY, BOILER BACKUP / HEAT PUMP OPTIMIZED, BOILER BACKUP / BOILER OPTIMIZED, BOILER BACKUP / BOILER PRIORITY, HEAT PUMP AND SOLAR BACKUP |
| HMI SetupState                | `$root/hmi/setupState`             | Enum   |      | OK, PARTIAL RESET, FACTORY RESET                                                                                                                                                       |
| PV Input Activated            | `$root/hmi/pvInputActivated`       | bool   |      |                                                                                                                                                                                        |
| Raw Message (Debug Mode Only) | `$root/hmi/debug`                  | string |      |                                                                                                                                                                                        |

### Main Message

| Value                              | MQTT Topic                            | Format | Unit | Other Information                                                   |
|------------------------------------|---------------------------------------|--------|------|---------------------------------------------------------------------|
| Present Error Code                 | `$root/main/errorCode`                | uint8  |      | 0 if there is no present error, else the error-code                 |
| Current Water Temperature          | `$root/main/waterTemp`                | float  | °C   |                                                                     |
| Supply Air Temperature             | `$root/main/supplyAirTemp`            | float  | °C   |                                                                     |
| Lower Evaporator Air Temperature   | `$root/main/evaporatorAirTempLower`   | float  | °C   |                                                                     |
| Upper Evaporator Air Temperature   | `$root/main/evaporatorAirTemp`        | float  | °C   |                                                                     |
| ~~Fan Speed~~	                     | ~~`$root/main/fanSpeed`~~	            | uint16 | rpm  | Removed since the message contained the pwm level and no rpm value. |   
| Fan-Level PWM                      | `$root/main/fanPWM`                   | float  | %    |                                                                     |
| State: Fan On/Off                  | `$root/main/stateFan`                 | bool   |      |                                                                     |
| State: Heatpump On/Off             | `$root/main/stateHeatpump`            | bool   |      |                                                                     |
| State: Heating Element On/Off      | `$root/main/stateElement`             | bool   |      |                                                                     |
| State: Boiler Backup On/Off        | `$root/main/stateExtBoiler`           | bool   |      |                                                                     |
| State: Defrost On/Off              | `$root/main/stateDefrost`             | bool   |      |                                                                     |
| State: Solar Input Signal Detected | `$root/main/stateSolar`               | bool   |      |                                                                     |
| State: PV Input Signal Detected    | `$root/main/statePV`                  | bool   |      |                                                                     |
| Setting: 1st Fan-Level PWM         | `$root/main/settingPWM_1`             | uint8  | %    |                                                                     |
| Setting: 2nd Fan-Level PWM         | `$root/main/settingPWM_2`             | uint8  | %    |                                                                     |
| Setting: 3rd Fan-Level PWM         | `$root/main/settingPWM_3`             | uint8  | %    |                                                                     |
| Setting: Min. Target Temp          | `$root/main/settingMinTargetTemp`     | uint8  | °C   |                                                                     |
| Setting: Min. Anti-Legionella Temp | `$root/main/settingMinLegionellaTemp` | uint8  | °C   |                                                                     |
| Setting: Wattage Heat Element      | `$root/main/settingWattageElement`    | uint16 | W    |                                                                     |
| Setting: Boiler Capacity           | `$root/main/settingBoilerCapacity`    | uint16 | l    |                                                                     |
| Setting: Boiler Brand              | `$root/main/settingBoilerBrand`       | Enum   |      | Atlantic, Thermor, Sauter, No Name                                  |
| Capability: Has Heat Exchanger     | `$root/main/capabilityHeatExchanger`  | bool   |      |                                                                     |
| Capability: Has Circulation        | `$root/main/capabilityCirculation`    | bool   |      |                                                                     |
| Capability: Has PV-Input           | `$root/main/capabilityPVInput`        | bool   |      |                                                                     |
| Capability: Has Communication      | `$root/main/capabilityCommunication`  | bool   |      |                                                                     |
| Capability: Has Anti-Dry-Heating   | `$root/main/capabilityAntiDryHeating` | bool   |      |                                                                     |
| Raw Message (Debug Mode Only)      | `$root/main/debug`                    | string |      |                                                                     |

### Energy Message

| Value                         | MQTT Topic                           | Format | Unit | Other Information                                                                                  |
|-------------------------------|--------------------------------------|--------|------|----------------------------------------------------------------------------------------------------|
| Total Heatpump Hours          | `$root/energy/totalHeatpumpHours`    | uint32 | h    | retained                                                                                           |
| Total Heating Element Hours   | `$root/energy/totalHeatingElemHours` | uint32 | h    | retained                                                                                           |
| Total Hours                   | `$root/energy/totalHours`            | uint32 | h    | retained                                                                                           |
| Total Energy                  | `$root/energy/totalEnergyWh`         | uint64 | Wh   |                                                                                                    |
| Total Water Consumption       | `$root/energy/waterConsumption`      | uint16 | l    | Note: Expected to wrap-around at UINT16_MAX                                                        |
| Current Power Heatpump        | `$root/energy/powerHeatpump`         | uint16 | W    | Note: It is possible to define an additional custom mqtt topic for this attribute within `Configuration.h` |
| Current Power Heating Element | `$root/energy/powerHeatingElem`      | uint16 | W    | Note: It is possible to define an additional custom mqtt topic for this attribute within `Configuration.h` |
| Current Power Total           | `$root/energy/powerTotal`            | uint16 | W    |                                                                                                    |
| Raw Message (Debug Mode Only) | `$root/energy/debug`                 | string |      |                                                                                                    |

### Error Messages

Collected Error Messages are provided to mqtt, the latest/recent occurred error is always error number == 1. The values
within the message reflect the state when the error has been created.

| Value                            | MQTT Topic                                      | Format | Unit | Other Information |
|----------------------------------|-------------------------------------------------|--------|------|-------------------|
| Error Number                     | `$root/error/%requestId/errorNumber`            | uint8  |      |                   |
| Error Code                       | `$root/error/%requestId/errorCode`              | uint8  |      |                   |
| Date                             | `$root/error/%requestId/date`                   | string |      | e.g. 25.5.2024    |
| Time (hh:ss)                     | `$root/error/%requestId/time`                   | string |      | e.g. 21:29        |
| Water Temperature                | `$root/error/%requestId/waterTemp`              | float  |      |                   |
| Lower Evaporator Air Temperature | `$root/error/%requestId/evaporatorAirTempLower` | float  |      |                   |
| Upper Evaporator Air Temperature | `$root/error/%requestId/evaporatorAirTemp`      | float  |      |                   |
| Fan-Level PWM                    | `$root/error/%requestId/fanPWM`                 | uint8  | %    |                   |
| Total Heatpump Hours             | `$root/error/%requestId/totalHeatpumpHours`     | uint16 |      |                   |
| Total Heating Element Hours      | `$root/error/%requestId/totalHeatingElemHours`  | uint16 |      |                   |
| Raw Message (Debug Mode Only)    | `$root/error/debug`                             | string |      |                   |

## Subscribe Topics

Using this topics you may override the HMI Controller in AquaMQTT OperationMode Man-In-The-Middle. Currently these are
the only ones implemented. If you need more overrides, feel free to raise an issue or even provide an PR.

| Value / Action                    | MQTT Topic                         | Format | Unit | Example Payload                                                      | Other Information                                                                                                                                                        |
|-----------------------------------|------------------------------------|--------|------|----------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Target Water Temperature          | `$root/ctrl/waterTempTarget`       | float  | °C   | "`55.0`"                                                             | Overrides the water temperature target: Allowed range is from 20°C to 62°C.                                                                                              |
| Operation Mode                    | `$root/ctrl/operationMode`         | Enum   |      | "`BOOST`"                                                            | Overrides the operation mode, may affect the waterTempTarget. For example BOOST and ABSENCE will automatically set the waterTempTarget accordingly.                      |
| Operation Type                    | `$root/ctrl/operationType`         | Enum   |      | "`ALWAYS ON`"                                                        | Overrides the operation type                                                                                                                                             |
| Installation Config               | `$root/ctrl/configInstallation`    | Enum   |      | "`HEAT PUMP ONLY`", "`BOILER BACKUP / HEAT PUMP PRIORITY"`", "`...`" | Overrides the installation config                                                                                                                                        |
| Enable or disable heating element | `$root/ctrl/heatingElementEnabled` | bool   |      | "`1`"                                                                | Allow the DHW heat pump to use the heating element if needed. Sanity: It is not possible to disable the heating element in case emergency mode is enabled.               |
| Enable or disable emergency mode  | `$root/ctrl/emergencyModeEnabled`  | bool   |      | "`0`"                                                                | Forces the DHW heat pump to use only the heating element. Sanity: it is not possible to enable emergency mode if heating element has been disabled.                      |
| Set PV Mode Heat Pump Flag        | `$root/ctrl/flagPVModeHeatPump`    | bool   |      | "`1`"                                                                | See [README-PV.md](/README-PV.md) additional information. Note: It is possible to define an additional custom mqtt topic for this attribute within `Configuration.h`     |
| Set PV Mode Heat Element Flag     | `$root/ctrl/flagPVModeHeatElement` | bool   |      | "`1`"                                                                | See [README-PV.md](/README-PV.md) for additional information. Note: It is possible to define an additional custom mqtt topic for this attribute within `Configuration.h` |
| Reset Overrides                   | `$root/ctrl/reset`                 | Void   |      |                                                                      | Removes all previous set overrides.                                                                                                                                      |

**Note:** Calling a `ctrl` topic with an empty payload `""` will reset individual override.