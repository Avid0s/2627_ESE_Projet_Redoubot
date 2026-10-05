/*
 * driver.c
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */


#include "driver.h"
#include "main.h"          // GPIO_NSLEEP_Pin / GPIO_NSLEEP_GPIO_Port (generated from the CubeMX label)

static const uint32_t ch_in1[2] = { DRIVER_L_CH_IN1, DRIVER_R_CH_IN1 };
static const uint32_t ch_in2[2] = { DRIVER_L_CH_IN2, DRIVER_R_CH_IN2 };

// A compare value of (ARR + 1) gives a 100 % duty cycle
static inline uint32_t pwm_full(void){
	return __HAL_TIM_GET_AUTORELOAD(&DRIVER_TIM) + 1;
}

static inline void set_inputs(driver_id_t d, uint32_t in1, uint32_t in2){
	__HAL_TIM_SET_COMPARE(&DRIVER_TIM, ch_in1[d], in1);
	__HAL_TIM_SET_COMPARE(&DRIVER_TIM, ch_in2[d], in2);
}

static inline uint32_t duty_to_ticks(uint16_t duty){
	if (duty > DRIVER_DUTY_MAX) duty = DRIVER_DUTY_MAX;
	return ((uint32_t)duty * pwm_full()) / DRIVER_DUTY_MAX;
}

void driver_init(void){
	// Outputs low before waking the drivers
	set_inputs(DRIVER_L, 0, 0);
	set_inputs(DRIVER_R, 0, 0);

	for (int i = 0; i < 2; i++) {
		HAL_TIM_PWM_Start(&DRIVER_TIM, ch_in1[i]);
		HAL_TIM_PWM_Start(&DRIVER_TIM, ch_in2[i]);
	}

	driver_sleep(false);
}

void driver_sleep(bool sleep){
	HAL_GPIO_WritePin(GPIO_NSLEEP_GPIO_Port, GPIO_NSLEEP_Pin, sleep ? GPIO_PIN_RESET : GPIO_PIN_SET);
	if (!sleep) HAL_Delay(2);        // DRV8833 wake-up time (~1 ms max)
}

void driver_forward(driver_id_t d, uint16_t duty){
	uint32_t t = duty_to_ticks(duty);
#if DRIVER_SLOW_DECAY
	set_inputs(d, pwm_full(), pwm_full() - t);   // drive: IN1=1 IN2=0, off phase: brake
#else
	set_inputs(d, t, 0);                         // drive: IN1=1 IN2=0, off phase: coast
#endif
}

void driver_reverse(driver_id_t d, uint16_t duty){
	uint32_t t = duty_to_ticks(duty);
#if DRIVER_SLOW_DECAY
	set_inputs(d, pwm_full() - t, pwm_full());
#else
	set_inputs(d, 0, t);
#endif
}

void driver_coast(driver_id_t d){
	set_inputs(d, 0, 0);
}

void driver_brake(driver_id_t d){
	set_inputs(d, pwm_full(), pwm_full());
}
