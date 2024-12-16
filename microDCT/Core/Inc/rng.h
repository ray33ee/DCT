/*
 * rng.h
 *
 *  Created on: Dec 14, 2024
 *      Author: Will
 */

#ifndef INC_RNG_H_
#define INC_RNG_H_

#include "main.h"

extern RNG_HandleTypeDef hrng;

struct RNG_State {
	uint32_t _state;
};

void rng_global_init();

void rng_global_seed(uint32_t);

uint32_t rng_global_next32();

#endif /* INC_RNG_H_ */
