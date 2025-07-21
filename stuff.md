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

# Next steps

- Voltage ramp up
- Controller GUI
- Virtual pins
- Figure out why UART doesn't work on start