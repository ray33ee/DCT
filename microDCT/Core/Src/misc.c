/*
 * mis.c
 *
 *  Created on: Dec 14, 2024
 *      Author: Will
 */

#include "misc.h"

void misc_delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim3,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim3) < us);  // wait for the counter to reach the us input in the parameter
}
