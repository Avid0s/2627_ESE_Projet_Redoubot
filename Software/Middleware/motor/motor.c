/*
 * motor.c
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */


#include "motor.h"

static const bool invert[2] = { MOTOR_L_INVERT, MOTOR_R_INVERT };

void motor_init(void){
	encoder_init();
	driver_init();
}

void motor_set_speed(motor_id_t m, int16_t speed){
	if (speed >  MOTOR_SPEED_MAX) speed =  MOTOR_SPEED_MAX;
	if (speed < -MOTOR_SPEED_MAX) speed = -MOTOR_SPEED_MAX;
	if (invert[m]) speed = -speed;

	driver_id_t d = (driver_id_t)m;
	if (speed >= 0) driver_forward(d, (uint16_t)speed);
	else            driver_reverse(d, (uint16_t)(-speed));
}

void motor_coast(motor_id_t m){
	driver_coast((driver_id_t)m);
}

void motor_brake(motor_id_t m){
	driver_brake((driver_id_t)m);
}

// Each motor: 30 % forward 1 s, brake, 30 % reverse 1 s, stop.
// Put a breakpoint after the forward phase and read encoder_get_total():
// it should be positive for both motors if the inversions are right.
void motor_test(void){
	for (int i = 0; i < 2; i++) {
		motor_id_t m = (motor_id_t)i;

		encoder_update((encoder_id_t)m);
		motor_set_speed(m, 300);
		HAL_Delay(1000);
		encoder_update((encoder_id_t)m);
		motor_brake(m);
		HAL_Delay(300);

		motor_set_speed(m, -300);
		HAL_Delay(1000);
		encoder_update((encoder_id_t)m);
		motor_coast(m);
		HAL_Delay(300);
	}
}
