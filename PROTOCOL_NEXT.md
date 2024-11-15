# Protocol (NEXT)

Another heat pump protocol observed in https://github.com/tspopp/AquaMQTT/issues/45 with an _Austria Email BWWP 200 WT SMART COZY_ heat pump.

## Message Format

```
ID LEN  PAYLOAD                                                                               CHECKSUM
C1  23  2E00230018001800150000000000000000000000008200000000454528CB000E0111                        44
C2  22  32120000001000062C01D0020000003A211E0C0C000000004E450000060422013E                          FA
43  2D  0000071D0000000000000000071D0000EEB2390000000000000000000B4C105AFB27FB250B1B000000000000    57
```

## Identifier

- 1 byte message identifier
- 1 byte payload length (length field and payload size)
- 1-254 bytes payload
- 1 bytes checksum

## Checksum

Checksum calculation is yet unknown. 

*TODO: A long time (e.g. two minutes) dump of AquaDebug is required*

## Messages

### HMI Message (194)

| Byte Number | Example (dec) | Purpose/Function               | Other Information |
|-------------|---------------|--------------------------------|-------------------|
| 0           | 34            | Length Field                   |                   |
| 1           | 50            | Water Target Temperature       |                   |
| 2           | 18            | Operation Mode, Operation Type | See table below   |
| 3           | 0             | ?                              |                   |
| 4           | 0             | Anti-Legionella Mode / AirDuct Mode | See table below |
| 5           | 0             | Emergency-Mode                 | 0 == inactive, 1 == active |
| 6           | 0             | InstallationConfig             | WP-Only == 0, WP+ExtBoiler-Prio-WP == 1, Wp+ExtBoiler-Opt-WP == 17, Wp+ExtBoiler-Opt-ExtBoiler == 33 , Wp+ExtBoiler-Prio-ExtBoiler == 49 , WP + Solar == 50|
| 7           | 0             | ExhaustFanConfig               | Only AirDuct EXT/INT 0 == off, 1 == slow, 2 == fast         |
| 8           | 6             | Heating-Element                | 0 == Automatic, 4 == locked, 6 ==  PV System "Smart-Grid" Yes  |
| 9           | 44            | Timer Mode: Window 1 Start     | Byte 1/2 --> See table below          |
| 10          | 1             | Timer Mode: Window 1 Start     | Byte 2/2 --> 44 + 256 = 300 / 60 = 5 --> 05:00        |
| 11          | 240           | Timer Mode: Window 1 Length    | Byte 1/2 --> See table below          |
| 12          | 0             | Timer Mode: Window 1 Length    | Byte 2/2 --> 240 + 0 = 240 / 60 = 4 --> 4h       |
| 13          | 14            | Absence Days                   | If the start is in the future, the value will only become visible then                  |
| 14          | 7             | Boost Days                     |  7 Days max       |
| 15          | 0             | ?                              |                   |
| 16          | 59            | Time Seconds                   |                   |
| 17          | 33            | Date Day, Month                |                   |
| 18          | 30            | Date Year, Month               |                   |
| 19          | 12            | Time Minutes                   |                   |
| 20          | 12            | Time Hours                     |                   |
| 21          | 8             | TestMode Status                | 1 == Heating-Element, 2 == Compressor, 8 == Fan, 32 == Defrost-Valve |
| 22          | 50            | TestMode Fanspeed              | 0 == standstill, 50 == slow, 100 == fast |
| 23          | 0             | ?                              |                   |
| 24          | 0             | ?                              |                   |
| 25          | 78            | ?                              |                   |
| 26          | 69            | ?                              |                   |
| 27          | 0             | ?                              |                   |
| 28          | 0             | ?                              |                   |
| 29          | 60            | Timer Mode: Window 2 Start     | Byte 1/2 --> See table below         |
| 30          | 0             | Timer Mode: Window 2 Start     | Byte 2/2 --> 60 + 0 = 60 / 60 = 1 == 01:00       |
| 31          | 240           | Timer Mode: Window 2 Length    | Byte 1/2 --> See table below         |
| 32          | 0             | Timer Mode: Window 2 Length    | Byte 2/2 --> 240 + 0 = 240 / 60 = 4 == 4h       |
| 33          | 62            | ?                              |                   |


##### Byte No. 2: Operation Mode

| Bit Number | Purpose/Function | Other Information                                                                      |
|------------|------------------|----------------------------------------------------------------------------------------|
| 0 - 3      | Operation Mode   | Interpreted as integer, 0 == Auto, 1 == ECO ON, 2 == ECO OFF, 5 == Absence, 9 == BOOST |
| 4 - 7      | Operation Type   | Interpreted as integer, 1 == Timer, 4 == Continious                                    |


##### Byte No. 4: Anti-Legionella Mode / AirDuct Mode

| Bit Number | Purpose/Function | Other Information                                                                      |
|------------|------------------|----------------------------------------------------------------------------------------|
| 0 - 3      | Anti-Legionella Mode   | Interpreted as integer, 0 == Off, 1 == 1perMonth, 2 == 2perMonth, 3 == 3perMonth, 4 == 4/perMonth |
| 4 - 7      | AirDuct Mode   | Interpreted as integer, 0 == AirDuct INT/INT, 1 == AirDuct EXT/INT, 2 == AirDuct EXT/EXT          |


##### Byte No. 9-12 + 29-32: Time Window:

Setting limitations:
| | Information|
|-------------|---------------|
| One Time Window | Minimum duration of 8 hours, maximum duration of 12 hours |
| Two Time Windows | Each at least 4 hours, combined maximum of 20 hours |
| Disable the second Time Window | Set the end time to less than 4 hours |


## Help Required:

- *TODO: OperationMode: ~~Boost, Eco On, Eco Off, Auto, Absence~~*
- *TODO: TimerWindow ~~A/B~~*
- *TODO: Anti-Legionalla ~~Setting~~*
- *TODO: AirDuct Config: ~~INT/INT, EXT/INT, EXT/EXT~~*
- *TODO: Installation Config: ~~IWP-Only, WP+ExtBoiler-Prio-WP,...~~*
- *TODO: Exhaust Fan Config:~~(Only in EXT/INT): STOP, LOW-SPEED, HIGH-SPEED~~*
- *TODO: Heating Element ~~Enabled On/Off/PV~~*
- *TODO: PV Mode allowed ~~On/Off~~*

### Main Message (193)

| Byte Number | Example (dec) | Purpose/Function | Other Information |
|-------------|---------------|------------------|-------------------|
| 0           | 35            | Length Field     |                   |
| 1           | 46            | Hot Water Temp                |                   |
| 2           | 0             | ?                |                   |
| 3           | 35            | Compressor Outlet Temp                |                   |
| 4           | 0             | ?                |                   |
| 5           | 24            | Upper Evaporator Temp                |                   |
| 6           | 0             | ?                |                   |
| 7           | 24            | Lower Evaporator Temp               |                   |
| 8           | 0             | ?                |                   |
| 9           | 21            | Input Air Temp                |                   |
| 10          | 0             | ?                |                   |
| 11          | 0             | ?                |                   |
| 12          | 0             | ?                |                   |
| 13          | 0             | ?                |                   |
| 14          | 0             | ?                |                   |
| 15          | 0             | ?                |                   |
| 16          | 0             | ?                |                   |
| 17          | 0             | Picture Bitmask  | See table below   |
| 18          | 50            | Fan-Speed        | 50 == low, 100 == high         |
| 19          | 0             | ?                |                   |
| 20          | 0             | ?                |                   |
| 21          | 0             | ?                |                   |
| 22          | 20            | State: PV and Solar Input | 20 == PV Enabled and Active, ?? == Solar Input Triggered |
| 23          | 0             | ?                |                   |
| 24          | 0             | ?                |                   |
| 25          | 0             | ?                |                   |
| 26          | 0             | ?                |                   |
| 27          | 69            | ?                |                   |
| 28          | 69            | ?                |                   |
| 29          | 40            | ?                |                   |
| 30          | 203           | ?                |                   |
| 31          | 0             | ?                |                   |
| 32          | 14            | ?                |                   |
| 33          | 1             | ?                |                   |
| 34          | 17            | ?                |                   |


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
| 2          | ? Boiler Backup On/Off   |                   |
| 3          | Fan On/Off.            |                   |
| 4          | ? Aktiv                     |                   |
| 5          | Defrost On/Off         |                   |
| 6          | ?                      |                   |
| 7          | ?                      |                   |

## Help Required:

_To identify this attributes within the message, try to locate the information within the HMI controller,
and provide a photo of the HMI controller together with a dump of the *main* message. We should find the values shown in the HMI controller within the message. 
For determining the values from super secret menu, you change a setting in the super secret menu of the hmi controller and watch the change of main message._

- *TODO: ~~Hot Water Temp~~*
- *TODO: ~~Input Air Temp~~*
- *TODO: ~~Lower Evaporator Temp~~*
- *TODO: ~~Upper Evaporator Temp~~*
- *TODO: PWM Level Settings (Super Secret Menu)* --> Option not available
- *TODO: Status Bitmask (Fan On/Off, Defrost On/Off, PV On/Off, Solar On/Off, HeatElement On/Off, HeatPump On/Off, Boiler Backup On/Off)*
- *TODO: Current Fan PWM Level* --> Only two levels possible: low and high
- *TODO: Min T Target (Super Secret Menu)* --> Option not available                                                                  
- *TODO: Anti-Legionella T Target (Super Secret Menu)* --> Option not available
- *TODO: Error Codes*
- *TODO: Wattage Heat Element (Super Secret Menu)* --> Option not available
- *TODO: Boiler Capacity (Super Secret Menu)* --> Option not available
- *TODO: Brand (Super Secret Menu)* --> Option not available
- *TODO: Setting Bitflags (Super Secret Menu)* --> Option not available

### Energy Message (67)

| Byte Number | Example (dec) | Purpose/Function | Other Information |
|-------------|---------------|------------------|-------------------|
| 0           | 45            | Length Field     |                   |
| 1           | 0             | ?                |                   |
| 2           | 0             | ?                |                   |
| 3           | 7             |  	Total Operation Hours (Heatpump)                |  Byte 1/2                 |
| 4           | 29            |  	Total Operation Hours (Heatpump)                |  Byte 2/2 --> (29x256) + 7 = 7431h   |
| 5           | 0             | ?                |                   |
| 6           | 0             | ?                |                   |
| 7           | 0             | ?                |                   |
| 8           | 0             | ?                |                   |
| 9           | 0             | ?                |                   |
| 10          | 0             | ?                |                   |
| 11          | 0             | ?                |                   |
| 12          | 0             | ?                |                   |
| 13          | 7             | Total Operation Hours (Heatpump)                |  The same as Byte 3+4 or for a reset?    |
| 14          | 29            | Total Operation Hours (Heatpump)                |  The same as Byte 3+4 or for a reset?    |
| 15          | 0             | ?                |                   |
| 16          | 0             | ?                |                   |
| 17          | 238           | ?                |                   |
| 18          | 178           | ?                |                   |
| 19          | 57            | ?                |                   |
| 20          | 0             | ?                |                   |
| 21          | 251           | Power Consumption Heatpump |  Byte 1/2                  |
| 22          | 1             | Power Consumption Heatpump  |  Byte 2/2 --> (1x256) + 151 = 507W ????                 |
| 23          | 0             | ?                |                   |
| 24          | 0             | ?                |                   |
| 25          | 0             | ?                |                   |
| 26          | 0             | ?                |                   |
| 27          | 0             | ?                |                   |
| 28          | 0             | ?                |                   |
| 29          | 11            | ?                |                   |
| 30          | 76            | ?                |                   |
| 31          | 16            | ?                |                   |
| 32          | 90            | ?                |                   |
| 33          | 251           | ?                |                   |
| 34          | 39            | ?                |                   |
| 35          | 251           | ?                |                   |
| 36          | 37            | ?                |                   |
| 37          | 11            | ?                |                   |
| 38          | 27            | ?                |                   |
| 39          | 0             | ?                |                   |
| 40          | 0             | ?                |                   |
| 41          | 0             | ?                |                   |
| 42          | 0             | ?                |                   |
| 43          | 0             | ?                |                   |
| 44          | 0             | ?                |                   |


## Help Required:

_To identify this attributes within the message, try to locate the information within the HMI controller,
and provide a photo of the HMI controller together with a dump of the *energy* message. We should find the values shown in the HMI controller within the message._

- *TODO: Power Consumption Heatpump* --> Cannot be displayed on the HMI       
- *TODO: Power Consumption Heating Element* --> Cannot be displayed on the HMI
- *TODO: Power Consumption Total (Both)*  -->  Cannot be displayed on the HMI       
- *TODO: Total Water Production (l)* -->  Cannot be displayed on the HMI           
- *TODO: ~~Total Operation Hours (Heatpump)~~*     
- *TODO: ~~Total Operation Hours (Heating Element)~~*
- *TODO: Total Operation Hours (Both)*  -->  Cannot be displayed on the HMI         
- *TODO: Total Energy Counter (Wh)* -->  Cannot be displayed on the HMI

### Error Message

*TODO: ~~Requires a running AquaDebug raw trace while opening the [super secret menu](https://github.com/tspopp/AquaMQTT/issues/17) in the hmi controller.~~*

### Not yet assignable

- *TODO: Number of restarts* 
- *TODO: Operating time (ready)*
