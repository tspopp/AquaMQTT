# Protocol (ODYSSEE)

Another heat pump protocol observed in https://github.com/tspopp/AquaMQTT/issues/68 with an _Atlantic Odyssee Pi_ heatpump (2015).

## Message Format

```
ID  LEN                                                                                            CRC
194 29   54 2   0  33 0  0 0   0   0   0   0  0   0  0 0   0 33 50 38 17 0 0 0 0 65 66 0 0          47
67  25   64 0 251  15 0  0 0   0 195   0   0  0 176 46 0   0  0  0  0  0 0 0 0 0                   240
217 31    0 0 233   0 2  0 127 1 237 235  88  0  14  1 194 0  0  1  0  0 2 0 0 0  0  0 0 0  0 0     26  
193 29   29 53  0  31 0 14 0  14   0  18   0  0   0 53 0  47  0  0  0  0 0 0 2 0  0  0 0 82 66       4
```

Error Message
```
74 68 46 24 54 0 75 0 23 0 24 0 35 0 0 0 54 0 55 0 65 3 0 0 0 0 0 0 0 0 81 38 0 0 152 0 0 0 123 12 0 0 47 13 0 0 152 26 0 0 46 1 0 0 221 46 58 12 1 24 0 0 0 0 0 0 0 0 43 128
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

| Byte Number | Example (dec) | Purpose/Function       | Other Information |
|-------------|---------------|------------------------|-------------------|
| 0           | 29            | Length Field           |                   |
| 1           | 54            |                        |                   |
| 2           | 2             |                        |                   |
| 3           | 0             |                        |                   |
| 4           | 33            |                        |                   |
| 5           | 0             |                        |                   |
| 6           | 0             |                        |                   |
| 7           | 0             |                        |                   |
| 8           | 0             |                        |                   |
| 9           | 0             |                        |                   |
| 10          | 0             |                        |                   |
| 11          | 0             |                        |                   |
| 12          | 0             |                        |                   |
| 13          | 0             |                        |                   |
| 14          | 0             |                        |                   |
| 15          | 0             |                        |                   |
| 16          | 0             |                        |                   |
| 17          | 33            |                        |                   |
| 18          | 50            |                        |                   |
| 19          | 38            |                        |                   |
| 20          | 17            |                        |                   |
| 21          | 0             |                        |                   |
| 22          | 0             |                        |                   |
| 23          | 0             |                        |                   |
| 24          | 0             |                        |                   |
| 25          | 65            |                        |                   |
| 26          | 66            |                        |                   |
| 27          | 0             | Error Number Requested |                   |
| 28          | 0             | Error Request Id       |                   |


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

| Byte Number | Example (dec) | Purpose/Function        | Other Information |
|-------------|---------------|-------------------------|-------------------|
| 0           | 31            | Length Field            |                   |
| 1           | 0             |                         |                   |
| 2           | 0             |                         |                   |
| 3           | 233           | Voltage Grid (?)        |                   |
| 4           | 0             | Voltage Grid (?)        |                   |
| 5           | 2             | Power Consumption Total |                   |
| 6           | 0             | Power Consumption Total |                   |
| 7           | 127           |                         |                   |
| 8           | 1             |                         |                   |
| 9           | 237           |                         |                   |
| 10          | 235           |                         |                   |
| 11          | 88            |                         |                   |
| 12          | 0             |                         |                   |
| 13          | 14            |                         |                   |
| 14          | 1             |                         |                   |
| 15          | 194           |                         |                   |
| 16          | 0             |                         |                   |
| 17          | 0             |                         |                   |
| 18          | 1             |                         |                   |
| 19          | 0             |                         |                   |
| 20          | 0             |                         |                   |
| 21          | 2             |                         |                   |
| 22          | 0             |                         |                   |
| 23          | 0             |                         |                   |
| 24          | 0             |                         |                   |
| 25          | 0             |                         |                   |
| 26          | 0             |                         |                   |
| 27          | 0             |                         |                   |
| 28          | 0             |                         |                   |
| 29          | 0             |                         |                   |
| 30          | 0             |                         |                   |

### Error Message (67)

| Byte Number | Example (dec) | Purpose/Function           | Other Information       |
|-------------|---------------|----------------------------|-------------------------|
| 0           | 68            | Length Field               |                         |
| 1           | 60            | Error Request Id           |                         |
| 2           | 24            | Error Code                 |                         |
| 3           | 47            | Eas chaude                 |                         |
| 4           | 0             | ?                          |                         |
| 5           | 73            | Refoulment compresseur     |                         |
| 6           | 0             | ?                          |                         |
| 7           | 20            | Evaporator haut            |                         |
| 8           | 0             | ?                          |                         |
| 9           | 19            | Evaporator bas             |                         |
| 10          | 0             | ?                          |                         |
| 11          | 35            | Air entrant                |                         |
| 12          | 0             | ?                          |                         |
| 13          | 0             | ?                          |                         |
| 14          | 0             | ?                          |                         |
| 15          | 47            | ECS haut                   |                         |
| 16          | 0             | ?                          |                         |
| 17          | 46            | ECS bas                    |                         |
| 18          | 0             | ?                          |                         |
| 19          | 97            | ?                          |                         |
| 20          | 2             | ?                          |                         |
| 21          | 0             | ?                          |                         |
| 22          | 0             | ?                          |                         |
| 23          | 0             | ?                          |                         |
| 24          | 0             | ?                          |                         |
| 25          | 0             | ?                          |                         |
| 26          | 0             | ?                          |                         |
| 27          | 0             | ?                          |                         |
| 28          | 0             | ?                          |                         |
| 29          | 130           | Tps fonct. comp 1er dem    |                         |
| 30          | 28            | Tps fonct. comp 1er dem    |                         |
| 31          | 0             | Tps fonct. comp 1er dem    |                         |
| 32          | 0             | Tps fonct. comp 1er dem    |                         |
| 33          | 125           | Tps fonct. AE 1er dem      |                         |
| 34          | 0             | Tps fonct. AE 1er dem      |                         |
| 35          | 0             | Tps fonct. AE 1er dem      |                         |
| 36          | 0             | Tps fonct. AE 1er dem      |                         |
| 37          | 158           | Nb. cycle fonct. PAC       |                         |
| 38          | 9             | Nb. cycle fonct. PAC       |                         |
| 39          | 0             | Nb. cycle fonct. PAC       |                         |
| 40          | 0             | Nb. cycle fonct. PAC       |                         |
| 41          | 48            | Nb. cycle fonct. FAN       |                         |
| 42          | 10            | Nb. cycle fonct. FAN       |                         |
| 43          | 0             | Nb. cycle fonct. FAN       |                         |
| 44          | 0             | Nb. cycle fonct. FAN       |                         |
| 45          | 131           | Nb. cycle fonct. VGC       |                         |
| 46          | 20            | Nb. cycle fonct. VGC       |                         |
| 47          | 0             | Nb. cycle fonct. VGC       |                         |
| 48          | 0             | Nb. cycle fonct. VGC       |                         |
| 49          | 0             | Nb. cycle fonct. AE        |                         |
| 50          | 1             | Nb. cycle fonct. AE        |                         |
| 51          | 0             | Nb. cycle fonct. AE        |                         |
| 52          | 0             | Nb. cycle fonct. AE        |                         |
| 53          | 6             | Date Day and Month         | Same Formula as in NEXT |
| 54          | 43            | Date Year                  | Same Formula as in NEXT |
| 55          | 26            | Time Minutes               |                         |
| 56          | 14            | Time Hours                 |                         |
| 57          | 1             | ?                          |                         |
| 58          | 3             | ? Valeur consigne a justée |                         |
| 59          | 0             | ?                          |                         |
| 60          | 0             | ?                          |                         |
| 61          | 0             | ?                          |                         |
| 62          | 0             | ?                          |                         |
| 63          | 0             | ?                          |                         |
| 64          | 0             | ?                          |                         |
| 65          | 0             | ?                          |                         |
| 66          | 0             | ?                          |                         |
| 67          | 32            | Nombre d'apparition        |                         |

