/*
 * pp.h
 *
 *	pp handles code for physical pins (HIO1-HIO16) including setup, reading and writing
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#ifndef INC_PP_H_
#define INC_PP_H_

#include "stm32g4xx_hal.h"
#include "mcp23017.h"

#define PP_COUNT (16)

enum PIN_STATUS {
	NC,
	INPUT,
	OUTPUT,
	VCC,
	GND,
};

struct PP_HANDLE {
	MCP23017_HANDLE_TD u2_handle;
	MCP23017_HANDLE_TD u3_handle;
};

void pp_init(struct PP_HANDLE*, I2C_HandleTypeDef*);

void pp_setup(struct PP_HANDLE*, enum PIN_STATUS*);

void pp_write(uint32_t, GPIO_PinState);

GPIO_PinState pp_read(uint32_t);

#endif /* INC_PP_H_ */
