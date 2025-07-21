/*
 * vp.h
 *
 *	Virtual Port
 *
 *	Virtual ports allow users so group any number of chosen pins into a single integer bitfield
 *
 *
 *	VP_DATA is represented by objects on the stack:
 *		First item: state - For inputs this is the last read value, for outputs this is the last value written
 *		Second item: Length - Number of pins used in the port (max 32)
 *		Third onwards: Pins: An array of `Length` port numbers, ordered LS to MS.
 *
 *	VP_DATA contains a single uint32_t* which points to a VP object (first item in stack). Lets say we have code like:
 *
 *	```
 *	port_a = VPO([P2, P3, P11, P1], 3)
 *  ```
 *
 *  This tells the controller to setup a virtual port in output mode on pins 2, 3, 11 and 1. The order here matters because these
 *  represent the first 4 bits of a number. The second argument is the initial state, so pins 2 and 3 will be set and 11 and 1 reset.
 *
 *	The stack will look a little something like
 *
 *	... 3, 4, 1, 2, 10, 0, ...
 *
 *	Where the first number, 3, is the initial state, 4 is the number of pins and the next 4 values are the pin numbers (from 0).
 *
 *	The VP_DATA object contains the address of the value 3, and obtains all other values relative to this.
 *
 *	port_a is assigned the address of the beginning of the VP data
 *
 *	Create a special instruction PUSHM K which is a negative offset from the current stack pointer and is used to calculate the
 *	position of the first element. This is then used to calculate a pointer, which is pushed onto the stack.
 *
 *	So the above example using `port_a` might translate to
 *
 *	PUSHI 3
 *	PUSHI 4
 *	PUSHI 1
 *	PUSHI 2
 *	PUSHI 10
 *	PUSHI 0
 *	PUSHM 5
 *
 *
 *
 *  Created on: Jul 21, 2025
 *      Author: Will
 */

#ifndef INC_VP_H_
#define INC_VP_H_


#include "stdint.h"
#include "pp.h"

struct VP_DATA {
	uint32_t* start;
};

uint32_t vp_get_pin_count(struct VP_DATA*);

uint32_t vp_get_state(struct VP_DATA*);

void vp_set_state(struct VP_DATA*, uint32_t);

uint32_t* vp_get_pins(struct VP_DATA*);

void vp_write_port(struct VP_DATA*, uint32_t);

uint32_t vp_read_port(struct VP_DATA*);

#endif /* INC_VP_H_ */
