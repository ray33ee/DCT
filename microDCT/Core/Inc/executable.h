/*
 * executable.h
 *
 *  Created on: Jul 11, 2025
 *      Author: Will
 */

#ifndef INC_EXECUTABLE_H_
#define INC_EXECUTABLE_H_

#include "stdint.h"
#include <stdio.h>
#include "uart.h"

struct Executable_State {
	uint8_t* rom;
	uint32_t rom_length;

};

void executable_init(struct Executable_State*);

void get_length_from_uart(struct Executable_State*);

void get_rom_from_uart(struct Executable_State*);

#endif /* INC_EXECUTABLE_H_ */
