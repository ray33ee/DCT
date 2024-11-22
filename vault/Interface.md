The interface between the primary and secondary CPUs is used to communicate information from the primary to the secondary (for example setting up the pin type or writing a logic value) and also from the secondary to the primary (for example reading a logic level)

The following protocols can be used:

- 1-Wire (Not ideal as no hardware support)
- I2C (Not ideal as each device would need its own different address baked into code)
- SPI (Not ideal as each device would need 4 pins)

Given a 20 pin MCU and 1 pin for VCC and one for GND, we have 18 GPIO. 15 are used for 5 pin interface circuits, which leaves 3 pins to talk to host.

The solution I propose is to use the I2C protocol with a simple chip select feature instead of address select which will mean each device requires only 3 pins.

you smell of bum