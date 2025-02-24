# MITM

## Message Sequence

### Legacy / Next
```mermaid
stateDiagram-v2
67
193
194
74: 74 (optional)

67 --> 193
193 --> 74
74 --> 194
193 --> 194
194 --> 67
```

### Odyssee
```mermaid
stateDiagram-v2
67
217
193
194
74: 74 (optional)

67 --> 217
217 --> 193
193 --> 194
194 --> 74
194 --> 67
74 --> 67
```

## Controller Task

Is responsible for the communication the DHW Controller. Replicates the HMI Controller.

```mermaid
flowchart LR
AquaMQTT <--> DHW(DHW Controller)
```
- Reads all controller messages and store message sequences from the controller e.g.
  - Legacy/Next: 67 - 193 - 74
  - Odysee: 67 - 217 - 193 
- If HMI trigger appears (194) check if it matches to the expected sequence and write HMI message to controller

## HMI Task

Is responsible for the communication with the HMI Controller. Replicates the Main Controller

```mermaid
flowchart LR
HMI(HMI Controller) <--> AquaMQTT
```
- Replicates message sequence and frequency according to the protocol type
- Requests HMI Message (194) from the HMI Controller
- If message contains request for error message (74), emit requested error message if already available

### State Machine

- Protocol expects a new message on the bus every 100ms
- Therefore the sum of SLEEP and SEND State must be 100ms
- Depending on the Protocol we need to send the error message pre or post message 194
- Protocol Odyssee is the only known protocol with message id 217

```mermaid
stateDiagram-v2
    REQUEST_194 --> SLEEP_194
    SLEEP_194 --> SEND_74POST: if PROTOCOL_ODYSSEE
    SLEEP_194 --> SEND_67: else
    SEND_74POST --> SLEEP_74POST
    SLEEP_74POST --> SEND_67
    SEND_67 --> SLEEP_67
    SLEEP_67 --> SEND_217: if PROTOCOL_ODYSSEE
    SLEEP_67 --> SEND_193: else
    SEND_217 --> SLEEP_217
    SLEEP_217 --> SEND_193
    SEND_193 --> SLEEP_193
    SLEEP_193 --> REQUEST_194: if PROTOCOL_ODYSSEE
    SLEEP_193 --> SEND_74PRE: else
    SEND_74PRE --> SLEEP_74PRE
    SLEEP_74PRE --> REQUEST_194


```