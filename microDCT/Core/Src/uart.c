/*
 * uart.c
 *
 *  Created on: Dec 14, 2024
 *      Author: Will
 */

#include "main.h"

extern UART_HandleTypeDef huart2;

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

uint32_t read_uart_into_buffer(uint8_t* buffer, uint32_t buffer_size, uint8_t stop_character) {
	//Keep reading into the until either a) the stop character is reached or b) we run out of buffer space
	uint32_t count = 0;

	while (1) {


		if (count >= buffer_size) {
			return -1;
		}

		__HAL_UART_CLEAR_OREFLAG(&huart2);

		HAL_UART_Receive(&huart2, buffer + count, 1, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, buffer + count, 1, HAL_MAX_DELAY);

		if (buffer[count] == stop_character) {
			buffer[count] = '\0';
			return count+1;
		}

		count += 1;
	}
}
