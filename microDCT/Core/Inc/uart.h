/*
 * uart.h
 *
 *  Created on: Dec 17, 2024
 *      Author: Will
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stdint.h"
#include "stm32g4xx_hal.h"

uint32_t read_uart_into_buffer(uint8_t*, uint32_t, uint8_t);

#endif /* INC_UART_H_ */
