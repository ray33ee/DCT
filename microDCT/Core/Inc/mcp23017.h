/*
 * mcp23015.h
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#ifndef INC_MCP23017_H_
#define INC_MCP23017_H_

#include "main.h"
extern I2C_HandleTypeDef hi2c1;

#define IODIRA_REGISTER_ADDRESS (0x00)

#define GPIOA_REGISTER_ADDRESS (0x12)

#define MCP_U2_ADDR (0b01000000)
#define MCP_U3_ADDR (0b01001110)

void mcp23017_init(uint16_t);

void mcp23017_gpio(uint16_t, uint16_t);


#endif /* INC_MCP23017_H_ */
