# Protocol

# Scope

This protocol is used by:

- Atlantic Explorer V5 (tested)
- Comfort by Sanibel CV5 (untested)

# Serial configuration

Serial interface configuration:

- Baud rate: 38400
- Configuration: 8N1

# Communication scheme

The protocol uses a request/response scheme, where HMI starts a transaction and MAIN responds. There are two types of
transactions: READ and WRITE. WRITE commands from HMI and READ responses from MAIN carry a payload.

On initialization, HMI sends a series of commands to exchange data. After that, HMI sends commands at a one-second interval.

## Frame layout

The frame layout is as follows:

- HEADER (5 bytes)
- PAYLOAD SIZE (1 byte, iff there is a payload)
- PAYLOAD (variable, iff there is a payload)
- CRC (2 bytes)

The header consists of:
- first byte: always `0x01`
- READ (`0x64`) or WRITE (`0x65`)
- command: parameter (two bytes)
- subfunction (one byte)

The payload format depends on the command.

All temperatures are degrees Celcius and encoded as 16 bits signed int (big endian), multiplied by 100.

Cycle status messages are count the number of seconds passed since a state change, and the total number of cycles. They have three 32bit numbers, and it can be in either of two states, `state0` or `state1`. The behavior is as follows:
- If in `state0`, the first number is the number of seconds in this state, and the second number is 0.
- If in `state1`, the second number is the number of seconds in this state, and the first number is 0.
- On transition from `state1` to `state0`, the third number is incremented by 1.

The version number and some other payloads are ASCII encoded text.

# Messages

| message | time | header | payload length | payload example | interpretation |
|---------|------|--------|----------------|-----------------|----------------|
| version number | init | 0164006401 | 17 | 322E390000000000000000000000000000 | '2.9' (ascii encoded text) |
| serial number? | init | 0164006501 | 13 | 313030***00 | '100***' (ascii encoded text) |
| heat pump serial number | init | 0164006601 | 16 | 313030***00 | '100***' (ascii encoded text) |
| power board version | init | 0164006701 | 17 | 312E352E31000000000000000000000000 | '1.5.1' (ascii encoded text) |
| model type? | init | 0164006E01 | 13 | 36303055***333000 | '600U***30' (ascii encoded text) |
| unknown | init | 0164007001 | 2 | 083F ||
| unknown | init | 0164007101 | 2 | 0315 ||
| unknown | init | 0164007501 | 1 | 02 ||
| unknown | init | 01640165FE | 0 || ?? |
| setpoint | init | 016414B701 | 2 | 1388 | 50°C |
| unknown | init | 0164152A01 | 2 | 0006 ||
| unknown | init | 0164158301 | 2 | 1838 | temperature 62°C |
| unknown | periodic | 016421B601 | 2 | 0000 ||
| unknown | init | 016443130D | 0 | | ?? |
| unknown | periodic | 0164FDED01 | 1 | 00 ||
| unknown | init | 0164FDFA01 | 1 | 05 | |
| unknown | init | 0164FDFD01 | 1 | 00 ||
| unknown | init | 0164FE0001 | 1 | 01 ||
| temperature sensor readings | periodic | 0164FEB006 | 12 | 135B 0ACA 0978 0886 0868 0906 | 49.55, 27.62, 24.24, 21.82, 21.52, 23.10 -> temperature of: water, compressor outlet, air inlet, evaporator 1-3 |
| water temperature (min/max) readings | periodic | 0164FEBA03 | 5 | 00 046B 13AB | 11.31, 50.35 -> display reads: 11, 51 |
| compressor outlet temperature (min/max) | readings periodic | 0164FEBD03 | 5 | 00 0525 1C1E | 13.17, 71.98 -> display reads: 13, 72 |
| air inlet temperature (min/max) readings | periodic | 0164FEC003 | 5 | 00 0318 0CCD | 7.92, 32.77 -> display reads: 7, 33 |
| evaporator 1 temperature (min/max) readings | periodic | 0164FEC303 | 5 | 00 FF47 120D |  -1.85, 46.21 -> display reads: -2, 47 |
| evaporator 2 temperature (min/max) readings | periodic | 0164FEC603 | 5 | 00 FF21 130D |  -2.23, 48.77 -> display reads: -3, 49 |
| evaporator 3 temperature (min/max) readings | periodic | 0164FEC903 | 5 | 00 FF29 117E |  -2.15, 44.78 -> display reads: -3, 45 |
| unknown | periodic | 0164FED801 | 1 | 00 | |
| unknown cycle status | periodic | 0164FEE203 | 12 | 00000000 0003E7C6 00000003 | 0, 255942 seconds, 3 cycles completed -> second counter of fourth cycle is active |
| unknown cycle status | periodic | 0164FEE503 | 12 | |
| unknown cycle status | periodic | 0164FEE803 | 12 | |
| unknown cycle status | periodic | 0164FEEB03 | 12 | |
| unknown cycle status | periodic | 0164FEEE03 | 12 | |
| unknown cycle status | periodic | 0164FEF103 | 12 | |
| input status | periodic | 0164FF1403 | 3 | 00 00 01 | off, off, on -> status of I2, I1, heat pump working? |
| unknown | init | 0164FFDC01 | 2 | 04B0 ||
| HMI version | init | 0165000301 | 17 | 322E352E33000000000000000000000000 | '2.5.3' (ascii encoded text) |
| model type? | init | 0165000A01 | 13 | 36303055***393000 | '600U***90' (ascii encoded text) |
| unknown | init | 0165152301 | 2 | 00C8 | boiler capacity in liters? |
| unknown | periodic | 016516B301 | 1 | 00 or 01 | |
| unknown | init | 0165FDF802 | 2 | 0007 ||
| unknown | init | 0165FDFB02 | 2 | 0011 ||
| unknown | init | 0165FDFE02 | 2 | 0001 ||
| unknown | periodic | 0165FEF701 | 1 | 00 ||
| unknown | incidental | 0165FEF901 | 1 | 00 or 64 | seems to correlate with heatpump activation |
| unknown | incidental | 0165FEFB01 | 1 | 00 or 64 | seems to correlate with heatpump activation |
| unknown | periodic | 0165FEFD01 | 1 | 00 ||
| unknown | periodic | 0165FEFF01 | 1 | 00 ||
| unknown | incidental | 0165FF0101 | 5 | 0000000000 or FFFFFFFF64 | seems to correlate with heatpump activation |
| unknown | incidental | 0165FF0301 | 1 | 00 or 2D | seems to correlate with heatpump activation |

# Transaction sequences

Some other messages below. The fields are:

- timestamp
- message type
- payload length
- payload value

Heat pump activation (water temperature 5°C under setpoint 50°C):
```
2026-05-04 09:19:51.968980,0165FEFB01,1,64
2026-05-04 09:19:52.156456,0165FF0301,1,2D

2026-05-04 09:20:51.913504,0165FF0101,5,FFFFFFFF64

2026-05-04 09:20:59.615906,0165FEF901,1,64
2026-05-04 09:20:59.798042,0165FF0101,5,0000000000
```

Heat pump deactivation (water temperature reaches setpoint at 50°C):
```
2026-05-04 10:23:33.120506,0165FEF901,1,00
2026-05-04 10:23:33.972909,0165FF0101,5,FFFFFFFF64

2026-05-04 10:23:41.861258,0165FF0101,5,0000000000
2026-05-04 10:23:41.889743,0165FF0301,1,00
2026-05-04 10:23:42.359572,0165FEFB01,1,00
```
