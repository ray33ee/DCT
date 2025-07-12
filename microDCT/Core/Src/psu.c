/*
 * psu.c
 *
 *  Created on: Dec 16, 2024
 *      Author: Will
 */

#include "psu.h"

uint32_t psu_generic_read(ADC_HandleTypeDef*);

void psu_init(
		struct PSU_STATE* psu_handle,
		ADC_HandleTypeDef* _adc_diff,
		ADC_HandleTypeDef* _adc_single,
		TIM_HandleTypeDef* _pwm_timer,
		volatile uint32_t* _ccr,
		uint32_t pwm_channel) {

	psu_handle->adc_diff = _adc_diff;
	psu_handle->adc_single = _adc_single;
	psu_handle->pwm_timer = _pwm_timer;

	psu_handle->ccr = _ccr;

	HAL_TIM_PWM_Start(_pwm_timer, pwm_channel);

	HAL_ADCEx_Calibration_Start(_adc_single, ADC_SINGLE_ENDED);

	HAL_ADCEx_Calibration_Start(_adc_diff, ADC_DIFFERENTIAL_ENDED);


	*_ccr = 0;
}

void psu_intensity(struct PSU_STATE* psu_handle, uint32_t intensity) {
	*psu_handle->ccr = intensity;
}

uint32_t psu_generic_read(ADC_HandleTypeDef* adc) {
	HAL_ADC_Start(adc);

	HAL_ADC_PollForConversion(adc, 100); // poll for conversion

	uint32_t r = HAL_ADC_GetValue(adc);

	return r;
}

uint32_t psu_output_voltage(struct PSU_STATE* psu_handle) {

	return psu_generic_read(psu_handle->adc_single);
}

uint32_t psu_shunt_diff(struct PSU_STATE* psu_handle) {

	return psu_generic_read(psu_handle->adc_diff);


}

float convert_adc_to_voltage(uint32_t raw_adc) {
	return (float)raw_adc / 4096.0 * 5.0;
}





