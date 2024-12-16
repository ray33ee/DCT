/*
 * soft_i2c.h
 *
 *  Created on: Dec 14, 2024
 *      Author: Will
 */

#ifndef INC_SOFT_I2C_H_
#define INC_SOFT_I2C_H_

#include "main.h"
#include "stm32g4xx_hal.h"

struct SOFT_I2C_HANDLE {
	GPIO_TypeDef *SDA_GPIO_port;
	uint16_t SDA_GPIO_pin;

	GPIO_TypeDef *SCL_GPIO_port;
	uint16_t SCL_GPIO_pin;

	uint32_t SCL_GPIO_POS;
	uint32_t SDA_GPIO_POS;

};

/* Set soft SDA and SCL to open drain outputs */
void soft_i2c_init(struct SOFT_I2C_HANDLE* i2c_handle, GPIO_TypeDef *sda_port, uint16_t sda_pin, GPIO_TypeDef *scl_port, uint16_t scl_pin, uint32_t sda_pos, uint32_t scl_pos);

/* Create the start condition */
void soft_i2c_start(struct SOFT_I2C_HANDLE* i2c_handle);

void soft_i2c_stop(struct SOFT_I2C_HANDLE* i2c_handle);

int soft_i2c_transmit(struct SOFT_I2C_HANDLE* i2c_handle, uint8_t address, uint8_t* data, uint32_t length);

GPIO_PinState soft_i2c_transmit_byte(struct SOFT_I2C_HANDLE* i2c_handle, uint8_t);

/* Send a single data bit */
void soft_i2c_transmit_bit(struct SOFT_I2C_HANDLE* i2c_handle, GPIO_PinState value);

uint8_t soft_i2c_receive_byte(struct SOFT_I2C_HANDLE* i2c_handle, GPIO_PinState ack);

/* Receive a single data bit */
GPIO_PinState soft_i2c_receive_bit(struct SOFT_I2C_HANDLE* i2c_handle);

/* Set the GPIO pin as an input */
void soft_i2c_input(GPIO_TypeDef *GPIO_port, uint32_t position);

/* Set the GPIO pin as an open drain output */
void soft_i2c_output(GPIO_TypeDef *GPIO_port, uint32_t position);

/* Set an output pin as high */
void soft_i2c_high(GPIO_TypeDef *GPIO_port, uint16_t GPIO_pin);

/* Reset an output pin as low */
void soft_i2c_low(GPIO_TypeDef *GPIO_port, uint16_t GPIO_pin);

/* Set or reset pin */
void soft_i2c_write(GPIO_TypeDef *GPIO_port, uint16_t GPIO_pin, GPIO_PinState state);

/* Read an inpt pin */
GPIO_PinState soft_i2c_read(GPIO_TypeDef *GPIO_port, uint16_t GPIO_pin);


#endif /* INC_SOFT_I2C_H_ */
