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

| Byte Number | Example (dec) | Purpose/Function               | Other Information |
|-------------|---------------|--------------------------------|-------------------|
| 0           | 34            | Length Field                   |                   |
| 1           | 50            | Water Target Temperature       |                   |
| 2           | 18            | Operation Mode, Operation Type | See table below   |
| 3           | 0             | ?                              |                   |
| 4           | 0             | ?                              |                   |
| 5           | 0             | ?                              |                   |
| 6           | 16            | ?                              |                   |
| 7           | 0             | ?                              |                   |
| 8           | 6             | ?                              |                   |
| 9           | 44            | ?                              |                   |
| 10          | 1             | ?                              |                   |
| 11          | 208           | ?                              |                   |
| 12          | 2             | ?                              |                   |
| 13          | 0             | ?                              |                   |
| 14          | 0             | ?                              |                   |
| 15          | 0             | ?                              |                   |
| 16          | 59            | Time Seconds                   |                   |
| 17          | 33            | Date Day, Month                |                   |
| 18          | 30            | Date Year, Month               |                   |
| 19          | 12            | Time Minutes                   |                   |
| 20          | 12            | Time Hours                     |                   |
| 21          | 0             | ?                              |                   |
| 22          | 0             | ?                              |                   |
| 23          | 0             | ?                              |                   |
| 24          | 0             | ?                              |                   |
| 25          | 78            | ?                              |                   |
| 26          | 69            | ?                              |                   |
| 27          | 0             | Error Number Requested         |                   |
| 28          | 0             | Error Request Id               |                   |
| 29          | 6             | ?                              |                   |
| 30          | 4             | ?                              |                   |
| 31          | 34            | ?                              |                   |
| 32          | 1             | ?                              |                   |
| 33          | 62            | ?                              |                   |


##### Byte No. 2: Operation Mode

| Bit Number | Purpose/Function | Other Information                                                        |
|------------|------------------|--------------------------------------------------------------------------|
| 0 - 3      | Operation Mode   | Interpreted as integer, 0 == Auto, 1 == ECO ON, 2 == ECO OFF, 9 == BOOST |
| 4 - 7      | Operation Type   | Interpreted as integer, 1 == Timer, 4 == Continious                      |



## Help Required:

- *TODO: OperationMode: ~~Boost, Eco On, Eco Off, Auto,~~ Absence*
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
| 0           | 35            | Length Field     |                   |
| 1           | 46            | ?                |                   |
| 2           | 0             | ?                |                   |
| 3           | 35            | ?                |                   |
| 4           | 0             | ?                |                   |
| 5           | 24            | ?                |                   |
| 6           | 0             | ?                |                   |
| 7           | 24            | ?                |                   |
| 8           | 0             | ?                |                   |
| 9           | 21            | ?                |                   |
| 10          | 0             | ?                |                   |
| 11          | 0             | ?                |                   |
| 12          | 0             | ?                |                   |
| 13          | 0             | ?                |                   |
| 14          | 0             | ?                |                   |
| 15          | 0             | ?                |                   |
| 16          | 0             | ?                |                   |
| 17          | 0             | ?                |                   |
| 18          | 0             | ?                |                   |
| 19          | 0             | ?                |                   |
| 20          | 0             | ?                |                   |
| 21          | 0             | ?                |                   |
| 22          | 130           | ?                |                   |
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

## Help Required:

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
| 0           | 45            | Length Field     |                   |
| 1           | 0             | ?                |                   |
| 2           | 0             | ?                |                   |
| 3           | 7             | ?                |                   |
| 4           | 29            | ?                |                   |
| 5           | 0             | ?                |                   |
| 6           | 0             | ?                |                   |
| 7           | 0             | ?                |                   |
| 8           | 0             | ?                |                   |
| 9           | 0             | ?                |                   |
| 10          | 0             | ?                |                   |
| 11          | 0             | ?                |                   |
| 12          | 0             | ?                |                   |
| 13          | 7             | ?                |                   |
| 14          | 29            | ?                |                   |
| 15          | 0             | ?                |                   |
| 16          | 0             | ?                |                   |
| 17          | 238           | ?                |                   |
| 18          | 178           | ?                |                   |
| 19          | 57            | ?                |                   |
| 20          | 0             | ?                |                   |
| 21          | 0             | ?                |                   |
| 22          | 0             | ?                |                   |
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

- *TODO: Power Consumption Heatpump*        
- *TODO: Power Consumption Heating Element*
- *TODO: Power Consumption Total (Both)*         
- *TODO: Total Water Production (l)*            
- *TODO: Total Operation Hours (Heatpump)*     
- *TODO: Total Operation Hours (Heating Element)*
- *TODO: Total Operation Hours (Both)*           
- *TODO: Total Energy Counter (Wh)*

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
