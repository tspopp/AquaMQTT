# PV Modes

AquaMqtt has been built to utilize local end excess energy produced by photovoltaic energy sources. For doing so, AquaMQTT provides additional operation modes to turn on either heat pump and/or heat element to reach the maximum water temperature. The additional operation modes, which are only available in MITM configuration, can be enabled using two flags which are signaled via [MQTT](./MQTT.md). These flags override various HMI attributes:

## Default

```C
flagPVModeHeatPump = false;
flagPVModeHeatElement = false;
```

Default AquaMQTT behaviour: AquaMQTT will use any overrides which have been previously set via MQTT control topics.

## PV Heat-Pump Only (PV HP)

```C
flagPVModeHeatPump = true;
flagPVModeHeatElement = false;
```

The heat pump will try to reach maximum allowed temperature using the heat pump, without usage of the heating element. This will consume between 300 and 550W of power, depending on the current water temperature and air temperature.

Active Overrides (Direction HMI to MAIN):
- Installation Mode is set to `HEATPUMP ONLY` (we don't want to use an external boiler if any connected)
- OperationMode is set to "`MAN ECO OFF`"
- OperationType is set to "`ALWAYS ON`"
- Water Temperature is set to `62°C`
- Heating Element is set to `disabled`
- Emergeny Mode is turned `off`
- Consumes about `300 - 550W`

Active Overrides (Direction MAIN to HMI):
- Solar State is set to `true` (shows Solar Icon on the HMI Display)

*Any other previously set overrides are ignored, if this mode is active!*

## PV Heat-Element Only (PV HE)

```C
flagPVModeHeatPump = false;
flagPVModeHeatElement = true;
```

The heat pump will try to reach maximum allowed temperate using the heating element, without usage of the heat pump. This will consume 1600W of power.

Active Overrides (Direction HMI to MAIN):
- Installation Mode is set to `HEATPUMP ONLY` (we don't want to use an external boiler if any connected)
- OperationMode is set to "`MAN ECO OFF`"
- OperationType is set to "`ALWAYS ON`"
- Water Temperature is set to `62°C`
- Heating Element is set to `enabled`
- Emergency Mode is turned `on` (forces usage of heat element)
- Consumes `1600W`

Active Overrides (Direction MAIN to HMI):
- PV State is set to `true` (shows PV Icon on the HMI Display)

*Any other previously set overrides are ignored, if this mode is active!*

## PV BOOST

```C
flagPVModeHeatPump = true;
flagPVModeHeatElement = true;
```

If both flags are set, the heat pump will enter the enter OperationMode "`BOOST`" and therefore utilize both heat pump and heat element to reach and keep maximum temperature.

Active Overrides (Direction HMI to MAIN):
- Installation Mode is set to `HEATPUMP ONLY` (we don't want to use an external boiler if any connected)
- OperationMode is set to "`BOOST`"
- OperationType is set to "`ALWAYS ON`"
- Water Temperature is set to `62°C`
- Heating Element is set to `enabled`
- Consumes `1600W` + `300 - 550W`

Active Overrides (Direction MAIN to HMI):
- Solar State is set to `true` (shows Solar Icon on the HMI Display)
- PV State is set to `true` (shows PV Icon on the HMI Display)

*Any other previously set overrides are ignored, if this mode is active!*

## How to trigger the flags?

The flags can be triggered using the [MQTT](./MQTT.md) control topics. The current PV mode and the status of the pv flags is also provided within the stats section.

| Value                            |                            MQTT Topic | Format |                                Other Information |
|----------------------------------|---------------------------------------|-------|--------------------------------------------------|
| Override Modes         | `$root/stats/overrideMode` | Enum |  `DEFAULT`, `PV HP`, `PV HE` or `PV BOOST`   |
| Flag PV heat pump      | `$root/stats/flagPVModeHeatPump` | bool | Status of the pv heat pump flag  |
| Flag PV heat element   | `$root/stats/flagPVModeHeatElement` | bool | Status of the pv heat element flag
| Set PV Mode Heat Pump Flag                 | `$root/ctrl/flagPVModeHeatPump`       |   bool           | Note: It is possible to define an additional custom mqtt topic for this attribute within `Configuration.h`  |
| Set PV Mode Heat Element Flag                 | `$root/ctrl/flagPVModeHeatElement`       |   bool           |  Note: It is possible to define an additional custom mqtt topic for this attribute within `Configuration.h`  |

## How to automate?

I am using my [OpenWB](https://github.com/snaptec/openWB) device for determining excess energy and turning on/off smart home devices. AquaMQTT is represented as two smart home devices in openWB, and is triggered automatically on various energy excess levels.

Any other solution triggering smart home devices on energy excess might work similar. You may provide custom mqtt topics to `Configuration.h` if needed.

```C++
constexpr char optionalPublishTopicHeatPumpCurrentPower[] = { "openWB/LegacySmartHome/set/Devices/1/Aktpower" };

constexpr char optionalPublishTopicHeatElementCurrentPower[] = { "openWB/LegacySmartHome/set/Devices/2/Aktpower" };

constexpr char optionalSubscribeTopicSetPvHeatPumpFlag[] = { "openWB/LegacySmartHome/set/Devices/1/ReqRelay" };

constexpr char optionalSubscribeTopicSetPvHeatElementFlag[] = { "openWB/LegacySmartHome/set/Devices/2/ReqRelay" };
```