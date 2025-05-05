# Protocol (NEXT)

Another heat pump protocol observed in https://github.com/tspopp/AquaMQTT/issues/45 with an _Austria Email BWWP 200 WT SMART COZY_ and _Atlantic Explorer v3_ heat pump. Further information in https://github.com/tspopp/AquaMQTT/pull/52.

For distinction with the original (LEGACY) implementation we refer to this Protocol as **Protocol NEXT**.

## Message Format

```
ID LEN  PAYLOAD                                                                                                                    CHECKSUM
C1  23  2E00230018001800150000000000000000000000008200000000454528CB000E0111                                                             44
C2  22  32120000001000062C01D0020000003A211E0C0C000000004E450000060422013E                                                               FA
43  2D  0000071D0000000000000000071D0000EEB2390000000000000000000B4C105AFB27FB250B1B000000000000                                         57
4A  41  0D0F2D00270015001800130000000000000000020000000000671D00000200000045110000581100007A2200000800000002000000211E000C00320000000008 6B
```

## Identifier

- 1 byte message identifier
- 1 byte payload length (length field and payload size)
- 1-254 bytes payload
- 1 bytes checksum

## Checksum

Is a regular XOR checksum on length field plus payload:

```c++
static uint8_t generateNextChecksum(const uint8_t* buffer, uint8_t length){
    int desiredChecksum = 0;
    // checksum is calculated on length field and payload
    for (size_t i = 0; i < length; i++)
    {
        // XOR the current byte with the accumulated checksum
        desiredChecksum ^= buffer[i];
    }
    return desiredChecksum;
}
```

## Messages

### HMI Message (194)

| Byte Number | Example (dec) | Purpose/Function                    | Other Information                                                                                                                                           |
|-------------|---------------|-------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 0           | 34            | Length Field                        |                                                                                                                                                             |
| 1           | 50            | Water Target Temperature            |                                                                                                                                                             |
| 2           | 18            | Operation Mode, Operation Type      | See table below                                                                                                                                             |
| 3           | 0             | ?                                   |                                                                                                                                                             |
| 4           | 0             | Anti-Legionella Mode / AirDuct Mode | See table below                                                                                                                                             |
| 5           | 0             | Emergency-Mode                      | 0 == inactive, 1 == active                                                                                                                                  |
| 6           | 0             | InstallationConfig                  | WP-Only == 0, WP+ExtBoiler-Prio-WP == 1, Wp+ExtBoiler-Opt-WP == 17, Wp+ExtBoiler-Opt-ExtBoiler == 33 , Wp+ExtBoiler-Prio-ExtBoiler == 49 , WP + Solar == 50 |
| 7           | 0             | ExhaustFanConfig                    | Only AirDuct EXT/INT 0 == off, 1 == slow, 2 == fast                                                                                                         |
| 8           | 6             | Heating-Element                     | 0 == Automatic, 4 == locked, 6 ==  PV System "Smart-Grid" Yes                                                                                               |
| 9           | 44            | Timer Mode: Window 1 Start          | Byte 1/2 --> See table below                                                                                                                                |
| 10          | 1             | Timer Mode: Window 1 Start          | Byte 2/2 --> 44 + 256 = 300 / 60 = 5 --> 05:00                                                                                                              |
| 11          | 240           | Timer Mode: Window 1 Length         | Byte 1/2 --> See table below                                                                                                                                |
| 12          | 0             | Timer Mode: Window 1 Length         | Byte 2/2 --> 240 + 0 = 240 / 60 = 4 --> 4h                                                                                                                  |
| 13          | 14            | Absence Days                        | If the start is in the future, the value will only become visible then                                                                                      |
| 14          | 7             | Boost Days                          | 7 Days max                                                                                                                                                  |
| 15          | 0             | Command                             | 0x02 -> Reset statistics                                                                                                                                    |
| 16          | 59            | Time Seconds                        |                                                                                                                                                             |
| 17          | 33            | Date Day, Month                     |                                                                                                                                                             |
| 18          | 30            | Date Year, Month                    |                                                                                                                                                             |
| 19          | 12            | Time Minutes                        |                                                                                                                                                             |
| 20          | 12            | Time Hours                          |                                                                                                                                                             |
| 21          | 8             | TestMode Status                     | 1 == Heating-Element, 2 == Compressor, 8 == Fan, 32 == Defrost-Valve                                                                                        |
| 22          | 50            | TestMode Fanspeed                   | 0 == standstill, 50 == slow, 100 == fast                                                                                                                    |
| 23          | 0             | ?                                   |                                                                                                                                                             |
| 24          | 0             | ?                                   |                                                                                                                                                             |
| 25          | 78            | ?                                   |                                                                                                                                                             |
| 26          | 69            | HMI Software Version                | Interpreted as ASCII, 69 == 'E'                                                                                                                             |
| 27          | 0             | Error Number Requested              |                                                                                                                                                             |
| 28          | 0             | Error Request Id                    |                                                                                                                                                             |
| 29          | 60            | Timer Mode: Window 2 Start          | Byte 1/2 --> See table below                                                                                                                                |
| 30          | 0             | Timer Mode: Window 2 Start          | Byte 2/2 --> 60 + 0 = 60 / 60 = 1 == 01:00                                                                                                                  |
| 31          | 240           | Timer Mode: Window 2 Length         | Byte 1/2 --> See table below                                                                                                                                |
| 32          | 0             | Timer Mode: Window 2 Length         | Byte 2/2 --> 240 + 0 = 240 / 60 = 4 == 4h                                                                                                                   |
| 33          | 62            | ?                                   |                                                                                                                                                             |


##### Byte No. 2: Operation Mode

| Bit Number | Purpose/Function | Other Information                                                                      |
|------------|------------------|----------------------------------------------------------------------------------------|
| 0 - 3      | Operation Mode   | Interpreted as integer, 0 == Auto, 1 == ECO ON, 2 == ECO OFF, 5 == Absence, 9 == BOOST |
| 4 - 7      | Operation Type   | Interpreted as integer, 1 == Timer, 4 == Continuous                                    |


##### Byte No. 4: Anti-Legionella Mode / AirDuct Mode

| Bit Number | Purpose/Function     | Other Information                                                                                 |
|------------|----------------------|---------------------------------------------------------------------------------------------------|
| 0 - 3      | Anti-Legionella Mode | Interpreted as integer, 0 == Off, 1 == 1perMonth, 2 == 2perMonth, 3 == 3perMonth, 4 == 4/perMonth |
| 4 - 7      | AirDuct Mode         | Interpreted as integer, 0 == AirDuct INT/INT, 1 == AirDuct EXT/INT, 2 == AirDuct EXT/EXT          |


##### Byte No. 9-12 + 29-32: Time Window:

Setting limitations:

|                                | Information                                               |
|--------------------------------|-----------------------------------------------------------|
| One Time Window                | Minimum duration of 8 hours, maximum duration of 12 hours |
| Two Time Windows               | Each at least 4 hours, combined maximum of 20 hours       |
| Disable the second Time Window | Set the end time to less than 4 hours                     |

### Main Message (193)

| Byte Number | Example (dec) | Purpose/Function                | Other Information                                                            |
|-------------|---------------|---------------------------------|------------------------------------------------------------------------------|
| 0           | 35            | Length Field                    |                                                                              |
| 1           | 46            | Hot Water Temp                  |                                                                              |
| 2           | 0             | ?                               |                                                                              |
| 3           | 35            | Compressor Outlet Temp          |                                                                              |
| 4           | 0             | ?                               |                                                                              |
| 5           | 24            | Upper Evaporator Temp           |                                                                              |
| 6           | 0             | ?                               |                                                                              |
| 7           | 24            | Lower Evaporator Temp           |                                                                              |
| 8           | 0             | ?                               |                                                                              |
| 9           | 21            | Input Air Temp                  |                                                                              |
| 10          | 0             | ?                               |                                                                              |
| 11          | 0             | ?                               |                                                                              |
| 12          | 0             | ?                               |                                                                              |
| 13          | 0             | ?                               |                                                                              |
| 14          | 0             | ?                               |                                                                              |
| 15          | 0             | ?                               |                                                                              |
| 16          | 0             | ?                               |                                                                              |
| 17          | 0             | Picture Bitmask                 | See table below                                                              |
| 18          | 50            | Fan-Speed                       | 50 == low, 100 == high                                                       |
| 19          | 0             | ?                               |                                                                              |
| 20          | 0             | ?                               |                                                                              |
| 21          | 0             | ?                               |                                                                              |
| 22          | 20            | State: PV and Solar Input       | 20 == PV Enabled and Active, ?? == Solar Input Triggered                     |
| 23          | 0             | ?                               |                                                                              |
| 24          | 0             | ?                               |                                                                              |
| 25          | 0             | ?                               |                                                                              |
| 26          | 0             | ?                               |                                                                              |
| 27          | 69            | Controller Software Version (?) | Interpreted as ASCII, 69 == 'E'                                              |
| 28          | 69            | Controller Software Version (?) | Interpreted as ASCII, 69 == 'E'. AquaMQTT currently uses this byte position. |
| 29          | 40            | ?                               |                                                                              |
| 30          | 203           | ?                               |                                                                              |
| 31          | 0             | ?                               |                                                                              |
| 32          | 14            | ?                               |                                                                              |
| 33          | 1             | ?                               |                                                                              |
| 34          | 17            | ?                               |                                                                              |

##### Byte No 17: Picture Bitmask

Findings...

```
0dec   | 0000 0000: Nothing Shown on HMI
24dec  | 0001 1000: Fan is turned on (observed via testmode)
? 8dec   | 0000 1001: Heating Element
26dec  | 0001 1010: Heatpump
27dec  | 0001 1011: Heatpump + Heating Element
? 14dec  | 0000 1110: Heatpump + Boiler Backup
? 15dec  | 0000 1111: Heatpump + Heating Element + Boiler Backup
32dec  | 0010 0000: Defrost
58dec  | 0011 1010: Defrost + Heatpump
? 40dec  | 0010 1000: Defrost + Fan
? 41dec  | 0010 1001: Defrost + Fan + Heating Element
? 64dec  | 0100 0000: Unknown, Observed while triggering Error 7
? 192dec | 1100 0000: Unknown, Observed while triggering Error 7
```

| Bit Number | Purpose/Function       | Other Information |
|------------|------------------------|-------------------|
| 0          | Heating Element On/Off |                   |
| 1          | Heatpump On/Off        |                   |
| 2          | ? Boiler Backup On/Off |                   |
| 3          | Fan On/Off.            |                   |
| 4          | ? Aktiv                |                   |
| 5          | Defrost On/Off         |                   |
| 6          | ?                      |                   |
| 7          | ?                      |                   |


## Help Required:

- *TODO: Error Codes*

### Energy Message (67)

| Byte Number | Example (dec) | Purpose/Function                                  | Other Information                           |
|-------------|---------------|---------------------------------------------------|---------------------------------------------|
| 0           | 45            | Length Field                                      |                                             |
| 1           | 0             | Total Operation Hours (Heat-Element) Resetable(?) |                                             |
| 2           | 0             | Total Operation Hours (Heat-Element) Resetable(?) |                                             |
| 3           | 7             | Total Operation Hours (Heatpump) Resetable(?)     | Byte 1/2                                    |
| 4           | 29            | Total Operation Hours (Heatpump)  Resetable(?)    | Byte 2/2 --> (29x256) + 7 = 7431h           |
| 5           | 0             | ?                                                 | Unknown Downcounting Value C during warm-up |
| 6           | 0             | ?                                                 | Unknown Downcounting Value C during warm-up |
| 7           | 0             | ?                                                 | Unknown Downcounting Value C during warm-up |
| 8           | 0             | ?                                                 | Unknown Downcounting Value C during warm-up |
| 9           | 0             | Total Operation Hours (Heat-Element) Permanent(?) |                                             |
| 10          | 0             | Total Operation Hours (Heat-Element) Permanent(?) |                                             |
| 11          | 0             | Total Operation Hours (Heat-Element) Permanent(?) |                                             |
| 12          | 0             | Total Operation Hours (Heat-Element) Permanent(?) |                                             |
| 13          | 7             | Total Operation Hours (Heatpump) Permanent(?)     | The same as Byte 3+4 or for a reset?        |
| 14          | 29            | Total Operation Hours (Heatpump) Permanent(?)     | The same as Byte 3+4 or for a reset?        |
| 15          | 0             | Total Operation Hours (Heatpump) Permanent(?)     |                                             |
| 16          | 0             | Total Operation Hours (Heatpump) Permanent(?)     |                                             |
| 17          | 238           | Total Energy (Wh)                                 |                                             |
| 18          | 178           | Total Energy (Wh)                                 |                                             |
| 19          | 57            | Total Energy (Wh)                                 |                                             |
| 20          | 0             | Total Energy (Wh)                                 |                                             |
| 21          | 251           | Power Consumption Total (Heatpump + Heat-Element) | Byte 1/2                                    |
| 22          | 1             | Power Consumption Total (Heatpump + Heat-Element) | Byte 2/2 --> (1x256) + 151 = 507W ????      |
| 23          | 0             | ?                                                 |                                             |
| 24          | 0             | ?                                                 |                                             |
| 25          | 0             | ?                                                 |                                             |
| 26          | 0             | ?                                                 |                                             |
| 27          | 0             | ?                                                 |                                             |
| 28          | 0             | ?                                                 |                                             |
| 29          | 11            | Water Temp Min                                    |                                             |
| 30          | 76            | Water Temp Max                                    |                                             |
| 31          | 16            | CompressorOutlet Temp Min                         |                                             |
| 32          | 90            | CompressorOutlet Temp Max                         |                                             |
| 33          | 251           | EvaporatorLower Air Temp Min                      |                                             |
| 34          | 39            | EvaporatorLower Air Temp Max                      |                                             |
| 35          | 251           | Evaporator Air Temp Min                           |                                             |
| 36          | 37            | Evaporator Air Temp Max                           |                                             |
| 37          | 11            | Air Temp Min                                      |                                             |
| 38          | 27            | Air Temp Max                                      |                                             |
| 39          | 0             | ?                                                 |                                             |
| 40          | 0             | ?                                                 |                                             |
| 41          | 0             | ?                                                 |                                             |
| 42          | 0             | ?                                                 |                                             |
| 43          | 0             | ?                                                 |                                             |
| 44          | 0             | ?                                                 |                                             |

### Error Message (74)

| Byte Number | Sample (dec) | Purpose/Function          | Other Information |
|-------------|--------------|---------------------------|-------------------|
| 0           | 65           | Length Field              |                   |
| 1           | 13           | RequestId                 |                   |
| 2           | 15           | ErrorCode                 |                   |
| 3           |              | ?                         |                   |
| 4           |              | ?                         |                   |
| 5           |              | ?                         |                   |
| 6           |              | ?                         |                   |
| 7           |              | ?                         |                   |
| 8           |              | ?                         |                   |
| 9           |              | ?                         |                   |
| 10          |              | ?                         |                   |
| 11          |              | ?                         |                   |
| 12          |              | ?                         |                   |
| 13          |              | ?                         |                   |
| 14          |              | ?                         |                   |
| 15          |              | ?                         |                   |
| 16          |              | ?                         |                   |
| 17          |              | ?                         |                   |
| 18          |              | ?                         |                   |
| 19          |              | ?                         |                   |
| 20          |              | ?                         |                   |
| 21          |              | ?                         |                   |
| 22          |              | ?                         |                   |
| 23          |              | ?                         |                   |
| 24          |              | ?                         |                   |
| 25          |              | ?                         |                   |
| 26          |              | ?                         |                   |
| 27          |              | ?                         |                   |
| 28          |              | ?                         |                   |
| 29          |              | ?                         |                   |
| 30          |              | ?                         |                   |
| 31          |              | ?                         |                   |
| 32          |              | ?                         |                   |
| 33          |              | ?                         |                   |
| 34          |              | ?                         |                   |
| 35          |              | ?                         |                   |
| 36          |              | ?                         |                   |
| 37          |              | ?                         |                   |
| 38          |              | ?                         |                   |
| 39          |              | ?                         |                   |
| 40          |              | ?                         |                   |
| 41          |              | ?                         |                   |
| 42          |              | ?                         |                   |
| 43          |              | ?                         |                   |
| 44          |              | ?                         |                   |
| 45          |              | ?                         |                   |
| 46          |              | ?                         |                   |
| 47          |              | ?                         |                   |
| 48          |              | ?                         |                   |
| 49          |              | ?                         |                   |
| 50          |              | ?                         |                   |
| 51          |              | ?                         |                   |
| 52          |              | ?                         |                   |
| 53          |              | ?                         |                   |
| 54          |              | ?                         |                   |
| 55          |              | ?                         |                   |
| 56          |              | ?                         |                   |
| 57          |              | ?                         |                   |
| 58          |              | ?                         |                   |
| 59          |              | ?                         |                   |
| 60          |              | ?                         |                   |
| 61          |              | ?                         |                   |
| 62          |              | ?                         |                   |
| 63          |              | ?                         |                   |
| 64          |              | ?                         |                   |

### Not yet assignable

- *TODO: Number of restarts* 
- *TODO: Operating time (ready)*
