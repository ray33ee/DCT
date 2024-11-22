User code is executed via a 64-bit 64 register VM. Instructions are:

Architecture:

VM uses 64 registers each with 64 bits (labelled r0-r64). Six (r0-r5) are used by the VM for special uses. 20 (r6-r25) are used to pass arguments to functions and to return values from functions. The remaining values (r26-r63) are used as local variables, and these are all you get as this VM does not support stack local variables. 

State:

Machine state is defined as 64 64-bit registers and a stack

- Math (0-19)
	- ADD
	- ADDI - Add immediate
	- SUB 
	- SUBI -Subtract immediate
	- INC
	- DEC
	- MUL
	- MULI - Multiply immediate
- Bitwise Logic (20-39)
	- OR 
	- ORI - Or Immediate
	- XOR
	- XORI - XOR immediate
	- AND
	- ANDI - And immediate
	- ONE - One's complement
	- NEG - Two's Complement
	- SHL - Shift left
	- SHR - Shift right
- Control flow (40-59)
	- JMP
	- JMPI
	- CALL
	- RET
	- CMP
	- CMPI - CMP immediate
	- JE
	- JNE
	- JL
	- JLE
	- JG
	- JGE
	- JZ
	- JNZ
- Data transfer (60-79)
	- MOV - Copy registers
	- MOVI - Load immediate
	- PUSH
	- POP
- GPIO setup (80-99)
	- SETUP - Setup pin 
	- READ - Read pin
	- WRITE - Write pin
- Timing (100-119)
	- DLA - Milliseconds delay
	- DLB - Microseconds delay
	- DLAI - Milliseconds delay, immediate
	- DLBI - Microseconds delay, immediate
- USI (SPI, UART, I2C, 1-WIRE) (120-139)
- Misc (200-250)
	- HALT
	- UBW - UART begin & wait
	- USR - UART send register
	- RNG
	- SEED

Bytecode:

Opcodes are 6 bit (upper 2 bits reserved for extended jumps), register indices are 1 byte, immediates are 4 byte, pin indices are 1 byte, pin setup codes are 1 byte, jump addresses are 34-bit (32 bit plus 2 bits borrowed from opcodes, allowing jumps in files spanning 16GB)