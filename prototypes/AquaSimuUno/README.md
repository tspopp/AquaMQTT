# AquaSimuUno

*Prototype*

Is a very lightweight heatpump bus simulation which is repeating the pattern 194 - 193 - 67 pattern.

The 194 message acts as a request to the hmi controller, which is completing the packet on the bus. 

The 193 and 67 messages are already complete and stimulate the hmi controller with a valid dataset.

**Successfully tested against the original HMI controller**