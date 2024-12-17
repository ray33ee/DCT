/*
 * soft_i2c.c
 *
 *  Created on: Dec 14, 2024
 *      Author: Will
 */

#include "soft_i2c.h"

#define HOLD_TIME (2) //t_{HD;STA}
#define SCL_HIGH_TIME (1) //t_{HD;STA}

/* Set soft SDA and SCL to open drain outputs */
void soft_i2c_init(struct SOFT_I2C_HANDLE* i2c_handle, GPIO_TypeDef *sda_port, uint16_t sda_pin, GPIO_TypeDef *scl_port, uint16_t scl_pin, uint32_t sda_pos, uint32_t scl_pos) {
	i2c_handle->SDA_GPIO_port = sda_port;
	i2c_handle->SDA_GPIO_pin = sda_pin;
	i2c_handle->SCL_GPIO_port = scl_port;
	i2c_handle->SCL_GPIO_pin = scl_pin;

	i2c_handle->SDA_GPIO_POS = sda_pos;
	i2c_handle->SCL_GPIO_POS = scl_pos;

	HAL_GPIO_WritePin(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_pin, GPIO_PIN_SET);
	// Set SCL to an output
	soft_i2c_output(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_POS);
	// Set SDA to an output
	soft_i2c_output(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_POS);

	uint32_t temp;

	/* Configure SDA as High frequency, no-pullup output open drain */

	/* Configure the IO Speed */
	temp = sda_port->OSPEEDR;
	temp &= ~(GPIO_OSPEEDR_OSPEED0 << (sda_pos * 2U));
	temp |= (GPIO_SPEED_FREQ_HIGH << (sda_pos * 2U));
	sda_port->OSPEEDR = temp;

	/* Configure the IO Output Type */
	temp = sda_port->OTYPER;
	temp &= ~(GPIO_OTYPER_OT0 << sda_pos) ;
	temp |= (((GPIO_MODE_OUTPUT_OD & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << sda_pos);
	sda_port->OTYPER = temp;

	/* Activate the Pull-up or Pull down resistor for the current IO */
	temp = sda_port->PUPDR;
	temp &= ~(GPIO_PUPDR_PUPD0 << (sda_pos * 2U));
	temp |= ((GPIO_NOPULL) << (sda_pos * 2U));
	sda_port->PUPDR = temp;

	/* Configure SCL as High frequency, no-pullup output open drain */

	/* Configure the IO Speed */
	temp = scl_port->OSPEEDR;
	temp &= ~(GPIO_OSPEEDR_OSPEED0 << (scl_pos * 2U));
	temp |= (GPIO_SPEED_FREQ_HIGH << (scl_pos * 2U));
	scl_port->OSPEEDR = temp;

	/* Configure the IO Output Type */
	temp = scl_port->OTYPER;
	temp &= ~(GPIO_OTYPER_OT0 << scl_pos) ;
	temp |= (((GPIO_MODE_OUTPUT_OD & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << scl_pos);
	scl_port->OTYPER = temp;

	/* Activate the Pull-up or Pull down resistor for the current IO */
	temp = sda_port->PUPDR;
	temp &= ~(GPIO_PUPDR_PUPD0 << (scl_pos * 2U));
	temp |= ((GPIO_NOPULL) << (scl_pos * 2U));
	scl_port->PUPDR = temp;
}

/* Create the start condition */
void soft_i2c_start(struct SOFT_I2C_HANDLE* i2c_handle) {

	soft_i2c_high(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_pin);
	soft_i2c_high(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_pin);

	misc_delay_us(HOLD_TIME);

	//Bring SDA low to initiate a start condition
	soft_i2c_low(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_pin);

	//Wait the hold time
	misc_delay_us(HOLD_TIME);

	//Then set SCL low to prepare for the first clock pulse
	soft_i2c_low(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_pin);

}

/* Create the stop condition */
void soft_i2c_stop(struct SOFT_I2C_HANDLE* i2c_handle) {

	soft_i2c_low(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_pin);
	soft_i2c_high(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_pin);

	misc_delay_us(HOLD_TIME);

	soft_i2c_high(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_pin);

}

int soft_i2c_transmit(struct SOFT_I2C_HANDLE* i2c_handle, uint8_t address, uint8_t* data, uint32_t length) {
	soft_i2c_output(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_POS);
	soft_i2c_output(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_POS);

	//Create START condition
	soft_i2c_start(i2c_handle);

	//Send address
	if (soft_i2c_transmit_byte(i2c_handle, address) != GPIO_PIN_RESET) {
		//NACK
		return -1;
	}

	for (int i = 0; i < length; i++) {
		if (soft_i2c_transmit_byte(i2c_handle, data[i]) != GPIO_PIN_RESET) {
			//NACK
			return -1*i-2;
		}
	}




	soft_i2c_stop(i2c_handle);

	return 0;
}

int soft_i2c_receive(struct SOFT_I2C_HANDLE* i2c_handle, uint8_t address, uint8_t* data, uint32_t length) {
	soft_i2c_output(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_POS);
	soft_i2c_output(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_POS);

	//Create START condition
	soft_i2c_start(i2c_handle);

	//Send address
	if (soft_i2c_transmit_byte(i2c_handle, address | 1) != GPIO_PIN_RESET) {
		//NACK
		return -1;
	}

	for (int i = 0; i < length; i++) {
		GPIO_PinState ack;
		if (i == length - 1) {
			ack = 1;
		} else {
			ack = 0;
		}

		data[i] = soft_i2c_receive_byte(i2c_handle, ack);
	}

	soft_i2c_stop(i2c_handle);


	return 0;

}

GPIO_PinState soft_i2c_transmit_byte(struct SOFT_I2C_HANDLE* i2c_handle, uint8_t byte) {

	for (int i = 0; i < 8; i++) {
		uint8_t bit = byte & 0b10000000;

		soft_i2c_transmit_bit(i2c_handle, bit);

		byte <<= 1;
	}

	soft_i2c_input(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_POS);

	GPIO_PinState ack = soft_i2c_receive_bit(i2c_handle);

	soft_i2c_low(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_pin);

	soft_i2c_output(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_POS);

	return ack;
}

/* Send a single data bit */
void soft_i2c_transmit_bit(struct SOFT_I2C_HANDLE* i2c_handle, GPIO_PinState value) {
	// Set or Reset SDA based on value
	soft_i2c_write(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_pin, value);
	//Clock SCL
	soft_i2c_high(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_pin);
	misc_delay_us(SCL_HIGH_TIME);
	soft_i2c_low(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_pin);

}

uint8_t soft_i2c_receive_byte(struct SOFT_I2C_HANDLE* i2c_handle, GPIO_PinState ack) {

	soft_i2c_input(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_POS);

	uint8_t rec = 0;

	for (int i = 7; i >= 0; i--) {
		if (soft_i2c_receive_bit(i2c_handle) != GPIO_PIN_RESET) {
			rec |= 1 << i;
		}
	}

	soft_i2c_output(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_POS);

	soft_i2c_transmit_bit(i2c_handle, ack);

	return rec;

}

/* Receive a single data bit */
GPIO_PinState soft_i2c_receive_bit(struct SOFT_I2C_HANDLE* i2c_handle) {
	soft_i2c_high(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_pin);

	//Read SDA
	GPIO_PinState value = soft_i2c_read(i2c_handle->SDA_GPIO_port, i2c_handle->SDA_GPIO_pin);

	//Clock SCL
	misc_delay_us(SCL_HIGH_TIME);
	soft_i2c_low(i2c_handle->SCL_GPIO_port, i2c_handle->SCL_GPIO_pin);

	return value;
}

/* Set the GPIO pin as an input */
void soft_i2c_input(GPIO_TypeDef *GPIO_port, uint32_t position) {

	uint32_t temp;

	/* Configure IO Direction mode as Output Open drain */
	temp = GPIO_port->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (position * 2U));
	temp |= ((GPIO_MODE_INPUT & GPIO_MODE) << (position * 2U));
	GPIO_port->MODER = temp;
}

/* Set the GPIO pin as an open drain output */
void soft_i2c_output(GPIO_TypeDef *GPIOx, uint32_t position) {

	uint32_t temp;

	/* Configure IO Direction mode as Output Open drain */
	temp = GPIOx->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (position * 2U));
	temp |= ((GPIO_MODE_OUTPUT_OD & GPIO_MODE) << (position * 2U));
	GPIOx->MODER = temp;

}

/* Set an output pin as high */
void soft_i2c_high(GPIO_TypeDef *GPIO_port, uint16_t GPIO_pin) {
	GPIO_port->BSRR = (uint32_t)GPIO_pin;
}

/* Reset an output pin as low */
void soft_i2c_low(GPIO_TypeDef *GPIO_port, uint16_t GPIO_pin) {
	GPIO_port->BRR = (uint32_t)GPIO_pin;
}

/* Set or reset pin */
void soft_i2c_write(GPIO_TypeDef *GPIO_port, uint16_t GPIO_pin, GPIO_PinState value) {
	if (value == GPIO_PIN_RESET) {
		GPIO_port->BRR = (uint32_t)GPIO_pin;
	} else {
		GPIO_port->BSRR = (uint32_t)GPIO_pin;
	}
}

/* Read an input pin */
GPIO_PinState soft_i2c_read(GPIO_TypeDef *GPIO_port, uint16_t GPIO_pin) {
	if ((GPIO_port->IDR & GPIO_pin) != 0x00U) {
		return GPIO_PIN_SET;
	}
	else {
		return GPIO_PIN_RESET;
	}
}

