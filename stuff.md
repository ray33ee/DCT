# Voltage rampup

 - Voltage set to 0
 - All pins are NC
 - All GND and VCC pins are set to GND
 - All VCC pins are set to VCC
 - Start current sensing and slowly ramp up voltave to desired level
 - If all is good, set IO pins to inputs or outputs then begin test

# Startup

 - Set PSU pwm to 0
 - Set all pins to NC

# Timeout

 - All tests must have a timeout, in ms, which if exceeded ends the program
 - For this to work instructions must be non-blocking
 - Return special error code from `execute` to show timeout exceeded

# Pin IO

 - Replace const calls like digitalWrite(3, LOW) and digitalWrite(0, HIGH) with reset_3() and set_0()
 - Replace const calls like digitalRead(2) with read_2()
 - Add pin names like PP1 and PP3

# Failure

 - Failures can return codes
 - Test writers can define a map which converts a failure code into a more meaningful failure message

# Voltage ramp up

 - Pins assigned to certain types will leek current (and increase ccurrent across shunt resistor)

# Virtual Pins

- Virutal pins are mostly implemented at the instruction level (we mmay need one more instruction to get the state of output values by storing the state and reading it)
- Needs to be implemented by the high level language. 
- If we assign to a VPO after initialisiation then we need to push the relevant index and v

# Next steps

- Voltage ramp up
- Controller GUI
- Virtual pins
- Figure out why UART doesn't work on start
- Add for loops of the simple form 

```Python

for i in 9:
	pass

```

with a single loop variable and a constant number iterator.

# Board 2

When the current pins are in certain modes, they drive the HV above 0 even when the PWM is off (select all pins as VCC then turn off psu_intensity). 

To solve this we can use transistors to cut off the 3.3 supply voltage (see simulations picX.txt for more info)

BE WARNED! Make sure the 3.3v supply is turned ON when the psu voltage comes up or the level shifter will not work and 5v could be sent to the STM GPIO LINES!!!!