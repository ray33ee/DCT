/*
 * mcp23017.c
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#include <mcp23017.h>


void mcp23017_init(uint16_t opcode) {
	const uint8_t CLEAR_GPIO_MESSAGE[] = {GPIOA_REGISTER_ADDRESS, 0x00, 0x00};
	const uint8_t SET_OUTPUT_MESSAGE[] = {IODIRA_REGISTER_ADDRESS, 0x00, 0x00};

	//Clear the GPIOA and GPIOB registers BEFORE we set the GPIO to outputs
	if (HAL_I2C_Master_Transmit(&hi2c1, opcode, CLEAR_GPIO_MESSAGE, sizeof(CLEAR_GPIO_MESSAGE), 10) != HAL_OK) {
		//error
	}

	//Set all the GPIO pins to output
	if (HAL_I2C_Master_Transmit(&hi2c1, opcode, SET_OUTPUT_MESSAGE, sizeof(SET_OUTPUT_MESSAGE), 10) != HAL_OK) {
		//error
	}
}

void mcp23017_gpio(uint16_t opcode, uint16_t gpio_a_b) {
	if (HAL_I2C_Master_Transmit(&hi2c1, opcode, (uint8_t*)&gpio_a_b, 2, 10) != HAL_OK) {
			//error
	}
}