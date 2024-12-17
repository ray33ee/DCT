/*
 * psu.h
 *
 *  Created on: Dec 16, 2024
 *      Author: Will
 */

#ifndef INC_PSU_H_
#define INC_PSU_H_

#include "stm32g4xx_hal.h"

struct PSU_STATE {
	ADC_HandleTypeDef* adc_single;
	ADC_HandleTypeDef* adc_diff;

	TIM_HandleTypeDef* pwm_timer;
	uint32_t* ccr;
	uint32_t channel;
};

void psu_init(struct PSU_STATE* psu_handle, ADC_HandleTypeDef*, ADC_HandleTypeDef*, TIM_HandleTypeDef*, volatile uint32_t*, uint32_t);

void psu_intensity(struct PSU_STATE*, uint32_t);

uint32_t psu_output_voltage(struct PSU_STATE*);

uint32_t psu_shunt_diff(struct PSU_STATE*);

float convert_adc_to_voltage(uint32_t);

#endif /* INC_PSU_H_ */
