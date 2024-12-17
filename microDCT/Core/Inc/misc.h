/*
 * misc.h
 *
 *  Created on: Dec 14, 2024
 *      Author: Will
 */

#ifndef INC_MISC_H_
#define INC_MISC_H_

#include "stm32g4xx_hal.h"
#include <stdint.h>


extern TIM_HandleTypeDef htim3;

void misc_delay_us(uint16_t);

#endif /* INC_MISC_H_ */
