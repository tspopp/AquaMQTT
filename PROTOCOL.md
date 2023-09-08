# Protocol 

This document reflects the current state of knowledge: **It may contain errors and misassumptions**. 

## General

- Protocol: Half-Duplex USART (TX and RX on a single wire)
- Baud-Rate: 9600bit/s
- Transfer-Bits: 8
- Stop-Bits: 2

### Wiring
The heatpumps HMI controller board is based upon a STM32F103RFT6 and provides a 8-pin (2x4 2,54mm) connector.

The original setup needs 4 of those pins, which are connected to the main controller:

- VCC 5V (green)
- GND (red/brown)
- TX/RX, Logical 5V (white)
- Unknown (yellow)

## Overview

The protocol uses an unknown binary frame format, with each message consisting of an unique identifier, payload length, payload and checksum.

### Message Format

- 1 byte message identifier
- 1 byte payload Length
- 1-255 bytes payload
- 2 bytes checksum (CRC-16)

Example Messages:
```
ID LEN ----------------------------------------------- PAYLOAD ------------------------------------------------------------------ CRC16..
193 37 235 1 121   0 94   0 102   0 0  0   0 0 1 100  65 81  0   0   0 50 62 0 255 255 255 87  9 66 88   2   0  16  14   1 78  23 254 243
194 35  18 2  66 252  0 240  17 240 6 16  56 0 0 255 255  0 30 147  46 11 11 0   0 255   0  9 66  1  1 255 255 255 255 255         74 199
 67 31   0 0   0   0  0   0   0   0 0  0 181 9 0   0  23  0  0   0 181  9  0 0 168 195  18  0  0  0  0   0                         25  82
```

### Identifier

*There are currently no insights, why they've chosen 193, 194 and 67 and if there is an additional but yet unknown meaning in these identifiers*

### Checksum

The two byte checksum value is created using standard CRC-16/CCITT-FALSE. The checksum is created for payload length and the actual payload itself: 

```
Example Message 67 in decimal and hexadecimal representation:

DEC: 31  0  0  0  0  0  0  0  0  0  0 181  9  0  0 23  0  0  0 181  9  0  0 168 195 18  0  0  0  0  0 25 82
HEX: 1F 00 00 00 00 00 00 00 00 00 00  B5 09 00 00 17 00 00 00  B5 09 00 00  A8  C3 12 00 00 00 00 00 19 52
```
In this message we expect an CRC of `0x1952` (last two bytes). Using the CRC-16/CCITT-FALSE algorithm we are able to successfully recreate the CRC values: [See online crc result using crccalc.com](https://crccalc.com/?crc=1F00000000000000000000B509000017000000B5090000A8C3120000000000&method=crc16&datatype=hex&outtype=0)

### Messages

Messages are either send from the main or the hmi controller. The main controller is the master of the communication and is repeating a pattern of message identifiers. The message sequence during normal operation is:

```
194 (HMI)
56ms idle
67 (MAIN)
60ms idle
193 (MAIN)
150ms idle
(repeated)
```

<img src="../media/sequence.png?raw=true" width=100% height=100%>

In case we disconnect the HMI controller during operation, we will notice that the 194 frame is no longer complete:

<img src="../media/nohmi1.png?raw=true" width=100% height=100%>

<img src="../media/nohmi2.png?raw=true" width=50% height=50%>

Therefore, message 194 is completed by the HMI controller, whereas 193 and 67 are emitted by the main controller.

### Send from the HMI controller

The main controller gives the HMI controller permission to send by emitting the 194 message type on the wire. The HMI controller completes the packet on the one-wire bus.

#### 194

| Byte Number | Example | Purpose/Function | Other Information |
|-------------|---------|------------------|-------------------|
| 0           |     35  | Length Field            | - |
| 1 - 2       |   18 2  | Target Temp.     | See Temperature Table, 53°C |
| 3           |     66  | OperationMode  | See Operation Mode Table |
| 4           |    252  | ?                | - |
| 5           |      0  | ?                | - |
| 6           |    240  | ?                | - |
| 7           |     17  | ?                | - |
| 8           |    240  | ?                | - |
| 9          |      4  | ?                | - |
| 10          |     16  | Timer Mode: Window 1 Start | 16 = 04:00h, 12 = 03:00h |
| 11          |     56  | Timer Mode: Window 1 Length  | 52 = 13h runtime, 56 = 14h runtime|
| 12          |      0  | Timer Mode: Window 2 Start  | e.g. 52 = 13:00h - Value 0x00 0x00 is supported if Timer 2 is not set.  |
| 13          |      0  | Timer Mode: Window 2 Length | e.g. 16 = 04h runtime. Max allowed length of both timers in sum is 14h runtime|
| 14          |    255  | ?                | - |
| 15          |    255  | ?                | - |
| 16          |      0  | ?                | - |
| 17          |     25  | Current Time Seconds | - |
| 18          |    151  | Current Day, Current Month in Half-Year  | See Formula below |
| 19          |     46  | Current Year, Half-Year | See Formula below  |
| 20          |     11  | Current Time Minutes                | - |
| 21          |     13  | Current Time Hour                | - |
| 22          |      0  | TestMode (?)                | HeatingElement, Heatpump, Defrost, Fan Hi/Lo |
| 23          |      0  | ?                | - |
| 24          |    255  | ?                | - |
| 25          |      0  | ?                | - |
| 26          |      9  | ?                | - |
| 27          |     66  | ?                | - |
| 28          |      0  | ?                | - |
| 29          |      0  | ?                | - |
| 30          |    255  | ?                | - |
| 31          |    255  | ?                | - |
| 32          |    255  | ?                | - |
| 33          |    255  | ?                | - |
| 34          |    255  | ?                | - |

##### Byte No. 3: Operation Mode


```
2dec  | 0000 0010:  AlwaysOn + ECO Inactive (?)
3dec  | 0000 0011:  AlwaysOn + Boost
64dec | 0100 0000:  TimerMode + Absence 
65dec | 0100 0001:  TimerMode + ECO Active
66dec | 0100 0010:  TimerMode + ECO Inactive
67dec | 0100 0011:  TimerMode + Boost
68dec | 0100 0100:  TimerMode + AUTO
```

Maybe:
```
Bit 3 - 0 interpreted as integer
0: Absence
1: Eco Active
2: Eco Inactive
3: Boost
4: Auto
```

```
Bit 6: TimerMode enabled
```

##### Temperature Values

Temperature values have an accurancy of a single decimal place. The two bytes are interpreted as an uint16 value and divided by 10:

``
float temperature = (float) (((int16_t) message[2] << 8) | message[1]) / 10.0;
``

Example Table:
```
184 1 = 44,0°C
194 1 = 45,0°C
204 1 = 46,0°C
214 1 = 47,0°C
224 1 = 48,0°C
234 1 = 49,0°C
244 1 = 50,0°C
254 1 = 51,0°C
8 2   = 52,0°C
18 2  = 53,0°C
28 2  = 54,0°C
38 2  = 55,0°C
48 2  = 56,0°C
58 2  = 57,0°C
68 2  = 58,0°C
78 2  = 59,0°C
88 2  = 60,0°C
98 2  = 61,0°C
108 2 = 62,0°C
```

##### Date Formula

```
    uint16_t year  = 2000 + (payload[19] / 2);
    uint8_t  month = 1 + (payload[18] >> 5) + ((payload[19] % 2) * 7);
    uint8_t  day   = payload[18] & 0x1F;
```

Example Table (Byte 19):

```
42dec = 2021 (First Half)	
43dec	= 2021	(Second Half)
44dec	= 2022	(First Half)
45dec	= 2022	(Second Half)
46dec	= 2023	(First Half)
47dec	= 2023	(Second Half)
```


### Send by the main controller

#### 193

| Byte Number | Example | Purpose/Function | Other Information |
|-------------|---------|------------------|-------------------|
| 0           |     37  | Length Field     | - |
| 1 - 2       |  235 1  | Hot Water Temp.  | Note: Range Allowed 18 - 62°C |
| 3 - 4       |  121 0  | Input Air Temp  | Note: Range Allowed 10 - 27°C |
| 5 - 6       |   94 0  | Lower Evaporator Temp | Note: Range Allowed -2 - 41°C |
| 7 - 8       |  102 0  | Upper Evaporator Temp | Note: Range Allowed -3 - 39°C |
| 9           |      0  | ?                | - |
| 10          |      0  | ?                | - |
| 11          |      0  | ?                | - |
| 12          |      0  | ?                | - |
| 13          |      1  | ?                | - |
| 14          |    100  | ?                | - |
| 15          |     65  | ?                | - |
| 16          |     81  | ?                | - |
| 17          |  8 or 0 | Picture Bitmask     | See table below |
| 18 - 19     |    0 0 | Fan-Speed        | Either 0 (off) or 650 (lowspeed) or 810 (highspeed) Maybe rpm?|
| 20          |     50  | ?                | - |
| 21          |     62  | ?                | - |
| 22          |      0  | ?                | - |
| 23          |    255  | ?                | - |
| 24          |    255  | ?                | - |
| 25          |    255  | ?                | - |
| 26          |     87  | ?                | - |
| 27          |      9  | ?                | - |
| 28          |     66  | ?                | - |
| 29          |     88  | ?                | - |
| 30          |      2  | ?                | - |
| 31          |      0  | ?                | - |
| 32          |     16  | ?                | - |
| 33          |     14  | ?                | - |
| 34          |      1  | ?                | - |
| 35          |     78  | ?                | - |
| 36          |     23  | ?                | - |


##### Byte No 17: Picture Bitmask

```
0dec  | 0000 0000: Nothing Shown on HMI
8dec  | 0000 1000: Fan is turned on (observed via testmode)
9dec  | 0000 1001: Heating Element
10dec | 0000 1010: Heatpump
11dec | 0000 1011: Heatpump + Heating Element
14dec | 0000 1110: Heatpump + Boiler Backup
15dec | 0000 1111: Heatpump + Heating Element + Boiler Backup
32dec | 0010 0000: Defrost
34dec | 0010 0010: Defrost + Heatpump
40dec | 0010 1000: Defrost + Fan
41dec | 0010 1001: Defrost + Fan + Heating Element
```

| Bit Number | Purpose/Function        | Other Information |
|------------|-------------------------|-------------------|
| 0           | Heating Element On/Off |  |
| 1           | Heatpump On/Off        |  |
| 2           | Boiler Backup On/Off    |  |
| 3           | Fan On/Off.            |  |
| 4           | ?                      |  |
| 5           | Defrost On/Off         |  |
| 6           | ?                      |  |
| 7           | ?                      |  |

#### 67

| Byte Number |             Example | Purpose/Function | Other Information |
|-------------|---------------------|------------------|-------------------|
| 0           |                  31 | Length Field                   | - |
| 1 - 2       |               221 1 | Power Consumption Heatpump | - |
| 3 - 4       |                 0 0 | Power Consumption Heating Element                | - |
| 5           |                   0 | ?                | - |
| 6           |                   0 | ?                | - |
| 7 - 8       |               221 1 | Power Consumption Total (Both)                | - |
| 9           |                  54 | Unknown Value A, First Byte | Is Counting upwards till 255 255, then reseting to 0 0. |
| 10          |                 215 | Unknown Value A, Second Byte| Seems value is not rising when heatpump is idle. Heating element has no influence |
| 11 - 14     |           231 9 0 0 | Total Operation Hours (Heatpump)  | - |
| 15 - 18     |            24 0 0 0 | Total Operation Hours (Heating Element) | - |
| 19 - 22     |           231 9 0 0 | Total Operation Hours (Both)        | - |
| 23 - 30     | 127 37 19 0 0 0 0 0 | Total Energy Counter (Wh)   | - |

