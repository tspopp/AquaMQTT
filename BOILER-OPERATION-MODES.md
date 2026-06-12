## Boiler Operation Modes

Atlantic allows for 4 normal operation modes for their boilers, with each their own use case. 
The nuances between each mode can be relevant or ignored depending of use case. There is also an 
absence mode.

The information here has been found though installers manuals and commercial information.
Some differences should be expected between generations of models.

### Operation Modes

1. **AUTO**
	Integrated algorithms take into account the data from the sensors to decide when it's 
	better to generate hot water and how. So the HMI is trying to optimise the usage of 
	heat-pump while using the heating element depending of what happened the previous days. 
	It accepts the time schedules defined by the user. It is unclear if MITM changes are well
	integrated in this mode.
	
2. **Manual ECO Off**
	Unit uses only the heat-pump to generate hot water, except if the outside air is too cold 
	or there has been a lot of water usage. There is probably an algorythm behind this mode. It
	also supposed to be allowing time programmations. Enabling this mode does not automatically
	start heating.
	
3. **Manual ECO On**
	Similar to Manual Eco Off, but with the Heatpump being enabled when the outside air is between 
	-5°C and 43°. The heat-element is then disabled. 
	
4. **BOOST**
	Directly starts the heating process. When toggled, both the heat-pump and heat-element are 
	enabled, except	if one of them has been flagged as disabled (like in the PV-modes). At least 
	some boilers have a limit set in their firmware that prevents the activation of heating if 
	the water temperature is above a certain value. If the heating process was already on, that 
	limit can be overtaken by the unit. Example: on the Explorer V4, this limit seems to be at 58°C.
	
5. **Absence**
	Water temperature is set at 15°C and will be heated using the resistance when dropping below
	this value. When setting the operation mode back to another mode, the boiler will directly 
	start to heat water. If the temperature is too low, it will enable the heating-element under 
	emergency-mode. 
