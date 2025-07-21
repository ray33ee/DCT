/*
 * mcp23017.c
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#include "mcp23017.h"



void mcp23017_init(struct MCP23017_HANDLE * mcp_handle, uint16_t opcode, I2C_HandleTypeDef* _hi2c) {
	const uint8_t CLEAR_GPIO_MESSAGE[] = {GPIOA_REGISTER_ADDRESS, 0x00, 0x00};
	const uint8_t SET_OUTPUT_MESSAGE[] = {IODIRA_REGISTER_ADDRESS, 0x00, 0x00};

	//Clear the GPIOA and GPIOB registers BEFORE we set the GPIO to outputs
	if (HAL_I2C_Master_Transmit(_hi2c, opcode, CLEAR_GPIO_MESSAGE, sizeof(CLEAR_GPIO_MESSAGE), 10) != HAL_OK) {
		//error
		printf("mcp23017_init clear registers error\n");
	}

	//Set all the GPIO pins to output
	if (HAL_I2C_Master_Transmit(_hi2c, opcode, SET_OUTPUT_MESSAGE, sizeof(SET_OUTPUT_MESSAGE), 10) != HAL_OK) {
		//error
		printf("mcp23017_init set pins to output error\n");
	}

	mcp_handle->hi2c = _hi2c;
}

void mcp23017_gpio(struct MCP23017_HANDLE * mcp_handle, uint16_t opcode, uint16_t gpio_a_b) {

	uint8_t a = gpio_a_b & 0xff;
	uint8_t b = gpio_a_b >> 8;

	const uint8_t WRITE_GPIO_MESSAGE_A[] = {GPIOA_REGISTER_ADDRESS, a};

	if (HAL_I2C_Master_Transmit(mcp_handle->hi2c, opcode, WRITE_GPIO_MESSAGE_A, sizeof(WRITE_GPIO_MESSAGE_A), 10) != HAL_OK) {
		//error
		printf("mcp23017_gpio write registers error\n");
	}

	const uint8_t WRITE_GPIO_MESSAGE_B[] = {GPIOB_REGISTER_ADDRESS, b};

	if (HAL_I2C_Master_Transmit(mcp_handle->hi2c, opcode, WRITE_GPIO_MESSAGE_B, sizeof(WRITE_GPIO_MESSAGE_B), 10) != HAL_OK) {
		//error
		printf("mcp23017_gpio write registers error\n");
	}


}
