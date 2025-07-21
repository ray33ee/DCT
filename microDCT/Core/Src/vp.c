/*
 * vp.c
 *
 *	Virtual port

 *  Created on: Jul 21, 2025
 *      Author: Will
 */

#include "vp.h"

uint32_t vp_get_state(struct VP_DATA* vp) {
	return *(vp->start);
}

void vp_set_state(struct VP_DATA* vp, uint32_t state) {
	*(vp->start) = state;
}

uint32_t vp_get_pin_count(struct VP_DATA* vp) {
	return *(vp->start+1);
}

uint32_t* vp_get_pins(struct VP_DATA* vp) {
	return vp->start + 2;
}

void vp_write_port(struct VP_DATA* vp, uint32_t value) {
	uint32_t len = vp_get_pin_count(vp);
	uint32_t* pins = vp_get_pins(vp);

	for (uint32_t i = 0; i < len; i++) {
		uint32_t pin_no = pins[i];

		if (value & 1) {
			pp_set(pin_no);
		} else {
			pp_reset(pin_no);
		}

		value >>= 1;

	}
}

uint32_t vp_read_port(struct VP_DATA* vp) {
	uint32_t len = vp_get_pin_count(vp);
	uint32_t* pins = vp_get_pins(vp);

	uint32_t value = 0;

	for (uint32_t i = 0; i < len; i++) {
		uint32_t pin_no = pins[len - i - 1];

		value <<= 1;

		if (pp_read(pin_no) != GPIO_PIN_RESET) {
			value |= 1;
		}


	}

	return value;
}
