/*
 * rng.h
 *
 *  Created on: Dec 14, 2024
 *      Author: Will
 */

#ifndef INC_RNG_H_
#define INC_RNG_H_

#include "stm32g4xx_hal.h"

struct RNG_State {
	uint32_t _state;
	RNG_HandleTypeDef* hrng;
};

void rng_global_init(RNG_HandleTypeDef*);

void rng_global_seed(uint32_t);

uint32_t rng_global_next32();

#endif /* INC_RNG_H_ */
