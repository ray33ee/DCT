/*
 * executable.c
 *
 *  Created on: Jul 11, 2025
 *      Author: Will
 */

#include "executable.h"

void executable_init(struct Executable_State* executable) {
	executable->rom = NULL;
	executable->rom_length = 0;
}

void get_length_from_uart(struct Executable_State* executable) {


	uint8_t buff[100];


	read_uart_into_buffer(buff, 100, '\n');

	uint32_t len = atoi(buff);

	//If len == 0 there is an issue

	executable->rom_length = len;
}

void get_rom_from_uart(struct Executable_State* executable) {
	free(executable->rom);

	executable->rom = malloc(executable->rom_length);

	//If executable->rom is null, allocation failed

	uint8_t buff[5];

	for (uint32_t i = 0; i < executable->rom_length; i++) {
		read_uart_into_buffer(buff, 5, '\n');

		uint32_t byte = atoi(buff);

		executable->rom[i] = (uint8_t)byte;
	}


}
