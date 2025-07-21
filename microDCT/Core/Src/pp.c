/*
 * pp.c
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#include "pp.h"

//Convert to 5 arrays to prevent multiplications

//Maps physical pins to their corresponding GS pins, HVS pins (both in MCP23017) and STM GPIO port/pin pair.
//For each physical pin we have:
//	MCP identifier (0 for U2 and 1 for U3)
//	GS pin (0-15, 0-7 is port A, 8-15 is port B)
//	HVS pin (0-15, 0-7 is port A, 8-15 is port B)
//	STM GPIO port (GPIO_TypeDef* cast to uint32_t)
//	STM GPIO pin
const uint32_t GS_HVS_SELECT_MAP[] = {
	1, 9, 8, (uint32_t)GPIOC, GPIO_PIN_10, //Pin 1
	1, 7, 6, (uint32_t)GPIOB, GPIO_PIN_9, //Pin 2
	1, 11, 10, (uint32_t)GPIOC, GPIO_PIN_11, //Pin 3
	1, 5, 4, (uint32_t)GPIOC, GPIO_PIN_5, //Pin 4
	1, 13, 12, (uint32_t)GPIOC, GPIO_PIN_12, //Pin 5
	1, 3, 2, (uint32_t)GPIOB, GPIO_PIN_6, //Pin 6
	1, 15, 14, (uint32_t)GPIOD, GPIO_PIN_2, //Pin 7
	1, 1, 0, (uint32_t)GPIOB, GPIO_PIN_11, //Pin 8

	0, 9, 8, (uint32_t)GPIOA, GPIO_PIN_4, //Pin 9
	0, 7, 6, (uint32_t)GPIOB, GPIO_PIN_10, //Pin 10
	0, 11, 10, (uint32_t)GPIOB, GPIO_PIN_0, //Pin 11
	0, 5, 4, (uint32_t)GPIOB, GPIO_PIN_15, //Pin 12
	0, 13, 12, (uint32_t)GPIOC, GPIO_PIN_2, //Pin 13
	0, 3, 2, (uint32_t)GPIOB, GPIO_PIN_14, //Pin 14
	0, 15, 14, (uint32_t)GPIOC, GPIO_PIN_3, //Pin 15
	0, 1, 0, (uint32_t)GPIOB, GPIO_PIN_5, //Pin 16

};

const uint32_t MCP_MAP[] = { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

const GPIO_TypeDef* STM_GPIO_PORT_MAP[] = {
	GPIOC,
	GPIOB,
	GPIOC,
	GPIOC,
	GPIOC,
	GPIOB,
	GPIOD,
	GPIOB,
	GPIOA,
	GPIOB,
	GPIOB,
	GPIOB,
	GPIOC,
	GPIOB,
	GPIOC,
	GPIOB,
};

const uint32_t STM_GPIO_PIN_MAP[] = {
	GPIO_PIN_10,
	GPIO_PIN_9,
	GPIO_PIN_11,
	GPIO_PIN_5,
	GPIO_PIN_12,
	GPIO_PIN_6,
	GPIO_PIN_2,
	GPIO_PIN_11,
	GPIO_PIN_4,
	GPIO_PIN_10,
	GPIO_PIN_0,
	GPIO_PIN_15,
	GPIO_PIN_2,
	GPIO_PIN_14,
	GPIO_PIN_3,
	GPIO_PIN_5,
};

/* Setup MCP expanders and set all STM GPIO to inputs */
void pp_init(struct PP_HANDLE* pp_handle, I2C_HandleTypeDef* hi2c) {

	//Setup both MCP devices
	mcp23017_init(&pp_handle->u2_handle, MCP_U2_ADDR, hi2c);
	mcp23017_init(&pp_handle->u3_handle, MCP_U3_ADDR, hi2c);

	//On startup, setup the device as though all pins are not connected. I.e. VCC off, GND off and STM GPIO as inputs
	for (int i = 0; i < PP_COUNT; i++) {
		pp_handle->pin_configs[i] = NC;
	}

	pp_setup(pp_handle);

}

/* Setup all physical pins as either input, output, vcc or gnd according to pp_handle->pin_configs */
void pp_setup(struct PP_HANDLE* pp_handle) {
	//Loop over all 16 pins
	//Extract relevant information via 'GS_HVS_SELECT_MAP'
	//Setup GS and HVS bits
	//Setup STM GPIO

	//Two 16 bit numbers represent all 4 banks of GPIO (port A and B for both MCPs)
	uint16_t mcp_gpio_registers[] = {0, 0};

	for (int i = 0; i < PP_COUNT; i++) {
		uint32_t mcp_index = GS_HVS_SELECT_MAP[i*5];
		GPIO_TypeDef* stm_gpio_port = STM_GPIO_PORT_MAP[i];
		uint16_t stm_gpio_pin = STM_GPIO_PIN_MAP[i];

		/* Set up the STM GPIO as an (input) or (output and set value) */
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		GPIO_InitStruct.Pin = stm_gpio_pin;

		if (pp_handle->pin_configs[i] == OUTPUT) {
			//Set all outputs to 0 by default
			HAL_GPIO_WritePin(stm_gpio_port, stm_gpio_pin, 0);

			//Setup gpio as input
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		} else {
			//If the PP is a vcc, gnd or input, set stm gpio as input
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
		}

		HAL_GPIO_Init(stm_gpio_port, &GPIO_InitStruct);

		if (pp_handle->pin_configs[i] == VCC) {
			uint32_t hvs_pin = GS_HVS_SELECT_MAP[i*5+2];

			mcp_gpio_registers[mcp_index] |= 1 << hvs_pin;
		} else if (pp_handle->pin_configs[i] == GND) {
			uint32_t gs_pin = GS_HVS_SELECT_MAP[i*5+1];

			mcp_gpio_registers[mcp_index] |= 1 << gs_pin;

		}
	}

	/* Write mcp_gpio_registers to MCP devices */
	mcp23017_gpio(&pp_handle->u2_handle, MCP_U2_ADDR, mcp_gpio_registers[0]);
	mcp23017_gpio(&pp_handle->u3_handle, MCP_U3_ADDR, mcp_gpio_registers[1]);

}

/* Write to a physical pin */
void pp_write(uint32_t pp_number, GPIO_PinState state) {

	if (state != GPIO_PIN_RESET)
	{
		pp_set(pp_number);
	}
	else
	{
		pp_reset(pp_number);
	}

}

/* Set physical pin */
void pp_set(uint32_t pp_number) {
	GPIO_TypeDef* stm_gpio_port = STM_GPIO_PORT_MAP[pp_number];
	uint16_t stm_gpio_pin = STM_GPIO_PIN_MAP[pp_number];

	stm_gpio_port->BSRR = (uint32_t)stm_gpio_pin;

}

/* Reset physical pin */
void pp_reset(uint32_t pp_number) {
	GPIO_TypeDef* stm_gpio_port = STM_GPIO_PORT_MAP[pp_number];
	uint16_t stm_gpio_pin = STM_GPIO_PIN_MAP[pp_number];

	stm_gpio_port->BRR = (uint32_t)stm_gpio_pin;

}

/* Read from a physical pin */
GPIO_PinState pp_read(uint32_t pp_number) {
	GPIO_TypeDef* stm_gpio_port = STM_GPIO_PORT_MAP[pp_number];
	uint16_t stm_gpio_pin = STM_GPIO_PIN_MAP[pp_number];

	return HAL_GPIO_ReadPin(stm_gpio_port, stm_gpio_pin);
}
