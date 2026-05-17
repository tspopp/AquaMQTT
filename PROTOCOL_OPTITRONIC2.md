# Optitronic 2 Serial Protocol

## Overview

The Optitronic 2 protocol is used by Austria Email / Groupe Atlantic heat pump water heaters with the "Optitronic 2" HMI controller. Unlike the other protocols supported by AquaMQTT (which use a proprietary serial format), the Optitronic 2 uses **standard Modbus RTU** on the serial bus between the HMI controller (master) and the main controller (slave).

### Compatible Devices

- Austria Email WPA 450 ECO
- Austria Email EKR 450 (with Optitronic 2 controller)
- Other Groupe Atlantic devices using the Optitronic 2 HMI (to be confirmed)

### Bus Parameters

| Parameter      | Value                                    |
|----------------|------------------------------------------|
| Wire protocol  | Modbus RTU, CRC-16/Modbus               |
| Baud rate      | 57600                                    |
| Data format    | 8N1                                      |
| Logic levels   | 5V TTL (via SN74LVC2T45 level shifter)  |
| Master         | HMI controller                           |
| Slave address  | `0x01` (main controller)                 |
| Polling rate   | ~1.7 Hz (5 read blocks per cycle)        |
| Write functions| `fn=0x06` (single register), `fn=0x10` (multiple registers) |

## Architecture

```
┌──────────┐     Modbus RTU      ┌──────────────┐
│   HMI    │◄───────────────────►│    Main      │
│ (Master) │     57600 8N1       │ (Slave 0x01) │
└──────────┘                     └──────────────┘
       │
       │  (passive tap via level shifter)
       │
┌──────────────┐
│   AquaMQTT   │
│   (ESP32)    │──── WiFi ──── MQTT Broker ──── Home Assistant
└──────────────┘
```

In **LISTENER** mode, AquaMQTT passively sniffs the bus and decodes both master requests (to track register addresses) and slave responses (to extract register data). It publishes decoded values to MQTT and accepts commands which are injected onto the bus during idle periods.

## Register Map

### Periodic Read Blocks

The HMI polls these blocks every ~600ms cycle:

| Block | Start  | Qty | Content                                        |
|-------|--------|-----|------------------------------------------------|
| 1     | 0x0000 | 25  | Settings + operating state                     |
| 2     | 0x00C8 | 10  | Sensor data (temperatures, PV status)          |
| 3     | 0x00E1 | 6   | Status flags                                   |
| 4     | 0x0197 | 1   | Unknown (always 0)                             |
| 5     | 0x0320 | 1   | Unknown (always 0)                             |

### On-Demand Blocks

Read only when user navigates specific menus:

| Block          | Start  | Qty | Trigger                  |
|----------------|--------|-----|--------------------------|
| DHW Schedule   | 0x0032 | 42  | Schedule menu            |
| Installer 1    | 0x015E | 25  | Installer PIN entry      |
| Installer 2    | 0x0190 | 25  | Installer PIN entry      |
| Vent Schedule  | 0x01C2 | 42  | Installer menu only      |

### User Settings (writable, fn=0x06)

| Register | Name                  | Encoding          | Default     |
|----------|-----------------------|-------------------|-------------|
| 0x0009   | DHW setpoint          | int16 ×0.1°C      | 550 (55°C)  |
| 0x000A   | ECO deviation         | signed int16 ×0.1°C | -100 (-10°C) |
| 0x000B   | Komfort deviation     | int16 ×0.1°C      | 20 (+2°C)   |
| 0x000C   | Program               | enum (1-4)        | 2 (ECO)     |
| 0x000D   | Aux heat source mode  | enum (1-3)        | 3 (both)    |
| 0x000F   | Force heating         | bool              | 0           |
| 0x0010   | Trigger anti-legionella | bool            | 0           |
| 0x0011   | External input function | enum (0-8)      | 6 (PV)      |
| 0x0016   | Quick-heat state      | int16 (bit15=activate) | 0x01B3 |

### Program Enum (reg 0x000C)

| Value | Name          |
|-------|---------------|
| 1     | NORMAL        |
| 2     | ECO           |
| 3     | KOMFORT       |
| 4     | KOMFORT PLUS  |

### External Input Function Enum (reg 0x0011)

| Value | Function                           |
|-------|------------------------------------|
| 0     | Schnellaufheizung (Quick Heat)     |
| 1     | OFF (remote shutdown)              |
| 2     | NORMAL                             |
| 3     | ECO                                |
| 4     | KOMFORT                            |
| 5     | KOMFORT PLUS                       |
| 6     | PHOTOVOLTAIK                       |
| 7     | Reservequelle                      |
| 8     | Funktionseingang 1                 |

### Sensor Registers (read-only, block 0x00C8+10)

| Register | Name                | Encoding      |
|----------|---------------------|---------------|
| 0x00C8   | Water temp (tank)   | int16 ×0.1°C  |
| 0x00CA   | Ambient/intake temp | int16 ×0.1°C  |
| 0x00CB   | Evaporator temp     | int16 ×0.1°C  |
| 0x00D1   | PV input status     | 0=inactive, 4=active |

### Operating State (from block 0x0000)

| Register | Name            | Encoding                      |
|----------|-----------------|-------------------------------|
| 0x0000   | Active setpoint | int16 ×0.1°C (effective)      |
| 0x0001   | Operating state | 2=idle/heating, 6=PV-boost    |
| 0x0016   | Quick-heat      | bit15=active, lower=target    |

### Quick-Heat Mechanism

Register 0x0016 serves dual purpose:
- **Read**: Current quick-heat target temperature (lower 15 bits × 0.1°C) + active flag (bit 15)
- **Write 0x81B3**: Activate quick-heat to 43.5°C (bit15 set + target)
- **Write 0x01B3**: Cancel quick-heat (just target, no bit15)

### PV Boost Detection

| Condition     | 0x0000 (setpoint) | 0x0001 (state) | 0x00D1 (PV) |
|---------------|-------------------|----------------|--------------|
| No PV         | 450 (45.0°C)      | 2              | 0            |
| PV active     | 700 (70.0°C)      | 6              | 4            |

Best indicator: `reg 0x00D1` → 0 = no PV, 4 = PV active.

### Installer Registers

| Register | Name                  | Default       |
|----------|-----------------------|---------------|
| 0x0160   | Frost protection temp | 70 (7.0°C)    |
| 0x019F   | PV target setpoint    | 700 (70.0°C)  |
| 0x01A0   | Bivalent threshold    | 30 (3.0°C)    |
| 0x01A3   | Ext source max temp   | 600 (60.0°C)  |

## MQTT Topics

All topics are prefixed with `aquamqtt/` (configurable via `mqttPrefix`).

### Published (read-only sensors)

| Topic                        | Type    | Description                    |
|------------------------------|---------|--------------------------------|
| `aquamqtt/waterTemp`         | float   | Tank water temperature (°C)    |
| `aquamqtt/supplyAirTemp`     | float   | Ambient/intake air temp (°C)   |
| `aquamqtt/evaporatorTemp`    | float   | Evaporator temperature (°C)    |
| `aquamqtt/statePV`           | bool    | PV input active (1/0)          |
| `aquamqtt/activeSetpoint`    | float   | Current effective setpoint (°C)|
| `aquamqtt/operatingState`    | string  | IDLE / PV_BOOST / UNKNOWN      |
| `aquamqtt/quickHeatActive`   | bool    | Quick heat active (1/0)        |

### Published (settings, also writable via ctrl/)

| Topic                        | Type    | Description                    |
|------------------------------|---------|--------------------------------|
| `aquamqtt/waterTempTarget`   | float   | DHW setpoint (°C)              |
| `aquamqtt/operationMode`     | string  | NORMAL/ECO/KOMFORT/KOMFORT_PLUS|
| `aquamqtt/auxHeatMode`       | string  | ELECTRIC/EXTERNAL/BOTH         |
| `aquamqtt/extInputFunction`  | string  | External input function        |
| `aquamqtt/forceHeating`      | bool    | Force heating active           |

### Control (subscribe to write)

| Topic                              | Payload                           |
|------------------------------------|-----------------------------------|
| `aquamqtt/ctrl/waterTempTarget`    | Temperature as float (35.0-70.0)  |
| `aquamqtt/ctrl/operationMode`      | NORMAL/ECO/KOMFORT/KOMFORT_PLUS   |
| `aquamqtt/ctrl/extInputFunction`   | Enum string (see above)           |
| `aquamqtt/ctrl/forceHeating`       | 1 or 0                            |
| `aquamqtt/ctrl/quickHeat`          | 1 (activate) or 0 (cancel)       |
| `aquamqtt/ctrl/triggerAntiLegionella` | 1                              |
| `aquamqtt/ctrl/reset`              | 1 (reboot ESP)                    |

## Configuration

To use the Optitronic 2 mode, set in `Configuration.h`:

```cpp
constexpr EOperationMode OPERATION_MODE = EOperationMode::OPTITRONIC2_LISTENER;
```

The serial configuration is fixed at 57600 baud, 8N1 (hardcoded in the Modbus listener task, as required by the Optitronic 2 protocol).

## Hardware Wiring

The wiring is simpler than the MITM mode — only one serial connection is needed (passive tap):

- Connect the ESP32 RX pin to the data line between HMI and main controller (via level shifter)
- For write support, also connect the TX pin to the same data line
- The SN74LVC2T45 level shifter from the standard AquaMQTT PCB works for this purpose

## Protocol Discovery

This protocol was reverse-engineered from an Austria Email WPA 450 ECO through two capture sessions (95 total captures) using an ESP32 TCP raw listener and custom decode/analysis tools. See the [device reference documentation](https://tome.corp.wal.tl:8080/knowledge/reference/austria-email-wpa450eco-reference) for the full capture analysis.
