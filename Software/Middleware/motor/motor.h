/*
 * motor.h
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_MIDDLEWARE_MOTOR_H_
#define SOFTWARE_MIDDLEWARE_MOTOR_H_

#include <stdint.h>
#include "driver.h"
#include "encoder.h"

typedef enum { MOTOR_L = 0, MOTOR_R = 1 } motor_id_t;

/* Speed is in per-mille of the full duty: -1000 (full reverse) .. +1000 (full forward).
 * Lower this if VM is above the 6 V rating of the motors. */
#define MOTOR_SPEED_MAX   1000

/* Flip so that a positive speed means "forward" for both sides */
#define MOTOR_L_INVERT    0
#define MOTOR_R_INVERT    1

void motor_init(void);                          // driver_init() + encoder_init()
void motor_set_speed(motor_id_t m, int16_t speed);
void motor_coast(motor_id_t m);
void motor_brake(motor_id_t m);

void motor_test(void);                          // blocking startup test, robot must be lifted

#endif /* SOFTWARE_MIDDLEWARE_MOTOR_H_ */
