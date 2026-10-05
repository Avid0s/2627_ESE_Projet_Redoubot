/*
 * servo.c
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#include "servo.h"

static int16_t servo_position = 0;

// Converts an angle (0..180) to a pulse width and writes it to one channel
static void servo_write(uint32_t channel, uint8_t angle){
	uint32_t pulse = SERVO_MIN + ((SERVO_MAX - SERVO_MIN) * angle) / 180;
	__HAL_TIM_SET_COMPARE(&SERVO_TIM, channel, pulse);
}

// Sets both servos (mirrored)
void servo_set_position(int16_t angle){
	if (angle < SERVO_ANGLE_MIN) angle = SERVO_ANGLE_MIN;
	if (angle > SERVO_ANGLE_MAX) angle = SERVO_ANGLE_MAX;

	servo_position = angle;
	servo_write(SERVO_CH_R, angle);
	servo_write(SERVO_CH_L, 180 - angle);
}

void servo_move(int16_t delta){
	servo_set_position(servo_position + delta);
}

int16_t servo_get_position(void){
	return servo_position;
}

// Startup test, blocking: min -> max -> min
void test_moteur(void){
	HAL_TIM_PWM_Start(&SERVO_TIM, SERVO_CH_R);
	HAL_TIM_PWM_Start(&SERVO_TIM, SERVO_CH_L);

	servo_set_position(SERVO_ANGLE_MIN);
	HAL_Delay(1000);                 // time to reach the minimum from any position

	servo_set_position(SERVO_ANGLE_MAX);
	HAL_Delay(1000);

	servo_set_position(SERVO_ANGLE_MIN);
	HAL_Delay(1000);
}
