/*
 * mcp23015.h
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#ifndef INC_MCP23017_H_
#define INC_MCP23017_H_

#include "stm32g4xx_hal.h"
#include "stdio.h"

#define IODIRA_REGISTER_ADDRESS (0x00)

#define GPIOA_REGISTER_ADDRESS (0x12)

#define MCP_U2_ADDR (0b01000000)
#define MCP_U3_ADDR (0b01001110)


typedef struct MCP23017_HANDLE {
	I2C_HandleTypeDef* hi2c;
} MCP23017_HANDLE_TD;

void mcp23017_init(struct MCP23017_HANDLE *, uint16_t, I2C_HandleTypeDef*);

void mcp23017_gpio(struct MCP23017_HANDLE *, uint16_t, uint16_t);


#endif /* INC_MCP23017_H_ */
