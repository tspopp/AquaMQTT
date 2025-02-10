# Protocol (ODYSSEE)

Another heat pump protocol observed in https://github.com/tspopp/AquaMQTT/issues/68 with an _Atlantic Odyssee Pi_ heatpump.

## Message Format

```
ID  LEN                                                                                            CRC
194 29   54 2   0  33 0  0 0   0   0   0   0  0   0  0 0   0 33 50 38 17 0 0 0 0 65 66 0 0          47
67  25   64 0 251  15 0  0 0   0 195   0   0  0 176 46 0   0  0  0  0  0 0 0 0 0                   240
217 31   31 0   0 232 0  1 0 129   1 236 210 88   0 14 1 204  0  0  1  0 0 2 0 0  0  0 0  0  0 0 0 208
193 29   29 53  0  31 0 14 0  14   0  18   0  0   0 53 0  47  0  0  0  0 0 0 2 0  0  0 0 82 66       4
```

## Identifier

Pattern is...
194 - 67 - 217 - 193

## Help Required:
- Determine is there also an error message in this sequence? (See below, triggering a error message)
- Determine frequency of messages (MITM)
- Determine who is sending message 217? (disconnect hmi controller and see if this message is still there)

## Checksum

Is a regular XOR checksum on length field plus payload, same as we have observed in NEXT.

```
Message 194 from above, expected checksum is 47 (0x2F):
dec: 29 54  2  0 33  0  0  0  0  0  0  0  0  0  0  0  0 33 50 38 17  0  0  0  0 65 66  0  0
hex: 1d 36 02 00 21 00 00 00 00 00 00 00 00 00 00 00 00 21 32 26 11 00 00 00 00 41 42 00 00
```
Paste hex here: https://www.scadacore.com/tools/programming-calculators/online-checksum-calculator/ and locate CheckSum8 Xor which is `2F`

## Messages

### HMI Message (194)

| Byte Number | Example (dec) | Purpose/Function | Other Information |
|-------------|---------------|------------------|-------------------|
| 0           | 29            | Length Field     |                   |
| 1           | 54            |                  |                   |
| 2           | 2             |                  |                   |
| 3           | 0             |                  |                   |
| 4           | 33            |                  |                   |
| 5           | 0             |                  |                   |
| 6           | 0             |                  |                   |
| 7           | 0             |                  |                   |
| 8           | 0             |                  |                   |
| 9           | 0             |                  |                   |
| 10          | 0             |                  |                   |
| 11          | 0             |                  |                   |
| 12          | 0             |                  |                   |
| 13          | 0             |                  |                   |
| 14          | 0             |                  |                   |
| 15          | 0             |                  |                   |
| 16          | 0             |                  |                   |
| 17          | 33            |                  |                   |
| 18          | 50            |                  |                   |
| 19          | 38            |                  |                   |
| 20          | 17            |                  |                   |
| 21          | 0             |                  |                   |
| 22          | 0             |                  |                   |
| 23          | 0             |                  |                   |
| 24          | 0             |                  |                   |
| 25          | 65            |                  |                   |
| 26          | 66            |                  |                   |
| 27          | 0             |                  |                   |
| 28          | 0             |                  |                   |


## Help Required:

- *TODO: OperationMode: Boost, Eco On, Eco Off, Auto,Absence*
- *TODO: TimerWindow A/B*
- *TODO: Anti-Legionalla Setting*
- *TODO: AirDuct Config: INT/INT, EXT/INT, EXT/EXT*
- *TODO: Installation Config: WP-Only, WP+ExtBoiler-Prio-WP,...*
- *TODO: Exhaust Fan Config: (Only in EXT/INT): STOP, LOW-SPEED, HIGH-SPEED*
- *TODO: Heating Element Enabled On/Off*
- *TODO: PV Mode allowed On/Off*


### Main Message (193)

| Byte Number | Example (dec) | Purpose/Function | Other Information |
|-------------|---------------|------------------|-------------------|
| 0           | 29            | Length Field     |                   |
| 1           |               |                  |                   |
| 2           |               |                  |                   |
| 3           |               |                  |                   |
| 4           |               |                  |                   |
| 5           |               |                  |                   |
| 6           |               |                  |                   |
| 7           |               |                  |                   |
| 8           |               |                  |                   |
| 9           |               |                  |                   |
| 10          |               |                  |                   |
| 11          |               |                  |                   |
| 12          |               |                  |                   |
| 13          |               |                  |                   |
| 14          |               |                  |                   |
| 15          |               |                  |                   |
| 16          |               |                  |                   |
| 17          |               |                  |                   |
| 18          |               |                  |                   |
| 19          |               |                  |                   |
| 20          |               |                  |                   |
| 21          |               |                  |                   |
| 22          |               |                  |                   |
| 23          |               |                  |                   |
| 24          |               |                  |                   |
| 25          |               |                  |                   |
| 26          |               |                  |                   |
| 27          |               |                  |                   |
| 28          |               |                  |                   |


_To identify this attributes within the message, try to locate the information within the HMI controller,
and provide a photo of the HMI controller together with a dump of the *main* message. We should find the values shown in the HMI controller within the message.
For determining the values from super secret menu, you change a setting in the super secret menu of the hmi controller and watch the change of main message._

- *TODO: Hot Water Temp*
- *TODO: Input Air Temp*
- *TODO: Lower Evaporator Temp*
- *TODO: Upper Evaporator Temp*
- *TODO: PWM Level Settings (Super Secret Menu)*
- *TODO: Status Bitmask (Fan On/Off, Defrost On/Off, PV On/Off, Solar On/Off, HeatElement On/Off, HeatPump On/Off, Boiler Backup On/Off)*
- *TODO: Current Fan PWM Level*
- *TODO: Min T Target (Super Secret Menu)*
- *TODO: Anti-Legionella T Target (Super Secret Menu)*
- *TODO: Error Codes*
- *TODO: Wattage Heat Element (Super Secret Menu)*
- *TODO: Boiler Capacity (Super Secret Menu)*
- *TODO: Brand (Super Secret Menu)*
- *TODO: Setting Bitflags (Super Secret Menu)*

### Energy Message (67)

| Byte Number | Example (dec) | Purpose/Function | Other Information |
|-------------|---------------|------------------|-------------------|
| 0           | 25            | Length Field     |                   |
| 1           |               |                  |                   |
| 2           |               |                  |                   |
| 3           |               |                  |                   |
| 4           |               |                  |                   |
| 5           |               |                  |                   |
| 6           |               |                  |                   |
| 7           |               |                  |                   |
| 8           |               |                  |                   |
| 9           |               |                  |                   |
| 10          |               |                  |                   |
| 11          |               |                  |                   |
| 12          |               |                  |                   |
| 13          |               |                  |                   |
| 14          |               |                  |                   |
| 15          |               |                  |                   |
| 16          |               |                  |                   |
| 17          |               |                  |                   |
| 18          |               |                  |                   |
| 19          |               |                  |                   |
| 20          |               |                  |                   |
| 21          |               |                  |                   |
| 22          |               |                  |                   |
| 23          |               |                  |                   |
| 24          |               |                  |                   |

## Help Required:

_To identify this attributes within the message, try to locate the information within the HMI controller,
and provide a photo of the HMI controller together with a dump of the *energy* message. We should find the values shown in the HMI controller within the message._

- *TODO: Power Consumption Heatpump*
- *TODO: Power Consumption Heating Element*
- *TODO: Power Consumption Total (Both)*
- *TODO: Total Water Production (l)*
- *TODO: Total Operation Hours (Heatpump)*
- *TODO: Total Operation Hours (Heating Element)*
- *TODO: Total Operation Hours (Both)*
- *TODO: Total Energy Counter (Wh)*

### Extra Message (217)

_Until we are not sure what this message is about, I call it "extra" message ;)

| Byte Number | Example (dec) | Purpose/Function | Other Information |
|-------------|---------------|------------------|-------------------|
| 0           | 31            | Length Field     |                   |
| 1           |               |                  |                   |
| 2           |               |                  |                   |
| 3           |               |                  |                   |
| 4           |               |                  |                   |
| 5           |               |                  |                   |
| 6           |               |                  |                   |
| 7           |               |                  |                   |
| 8           |               |                  |                   |
| 9           |               |                  |                   |
| 10          |               |                  |                   |
| 11          |               |                  |                   |
| 12          |               |                  |                   |
| 13          |               |                  |                   |
| 14          |               |                  |                   |
| 15          |               |                  |                   |
| 16          |               |                  |                   |
| 17          |               |                  |                   |
| 18          |               |                  |                   |
| 19          |               |                  |                   |
| 20          |               |                  |                   |
| 21          |               |                  |                   |
| 22          |               |                  |                   |
| 23          |               |                  |                   |
| 24          |               |                  |                   |
| 25          |               |                  |                   |
| 26          |               |                  |                   |
| 27          |               |                  |                   |
| 28          |               |                  |                   |
| 29          |               |                  |                   |
| 30          |               |                  |                   |

### Error Message (TODO)

## Help Required:

_Error messages can be triggered when opening the super secret engineering menu. Do a dump with AquaDebug while opening the super secret menu and we should be able to determine the error message_