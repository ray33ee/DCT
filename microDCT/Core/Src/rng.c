/*
 * rng.c
 *
 *	rng unit uses xorshift32 to generate a sequence of pseudo random numbers seeded from the RNG peripheral
 *
 *  Created on: Dec 14, 2024
 *      Author: Will
 */

#include "rng.h"

struct RNG_State GLOBAL_RNG_STATE;

/* Initialise the rng state with a seed from the RNG peripheral */
void rng_global_init() {
	if (HAL_RNG_GenerateRandomNumber(&hrng, &(GLOBAL_RNG_STATE._state)) != HAL_OK) {
		//error
	}

	//Zero is a fixed point so we map it to 1 instead
	if (GLOBAL_RNG_STATE._state == 0) {
		GLOBAL_RNG_STATE._state = 1;
	}
}

/* Function to allow user to use their own seed for reproducability */
void rng_global_seed(uint32_t seed) {
	if (seed == 0) { //Zero is a fixed point so we map it to 1 instead
		GLOBAL_RNG_STATE._state = 1;
	} else {
		GLOBAL_RNG_STATE._state = seed;
	}
}

/* Use xorshift32 to generate the next seed */
uint32_t rng_global_next32() {
	GLOBAL_RNG_STATE._state ^= GLOBAL_RNG_STATE._state << 13;
	GLOBAL_RNG_STATE._state ^= GLOBAL_RNG_STATE._state >> 17;
	GLOBAL_RNG_STATE._state ^= GLOBAL_RNG_STATE._state << 5;
	return GLOBAL_RNG_STATE._state;
}
