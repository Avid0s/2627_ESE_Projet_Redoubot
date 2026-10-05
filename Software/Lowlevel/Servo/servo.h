/*
 * servo.h
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 *
 *  Two mirrored servos (DFRobot SER0039) on TIM15 used as a gripper.
 */

#ifndef SOFTWARE_CORE_SERVO_SERVO_H_
#define SOFTWARE_CORE_SERVO_SERVO_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "main.h"

/* Pulse width in microseconds (timer tick = 1 us, period = 20 ms) */
#define SERVO_MIN    1000             // ~0 degrees
#define SERVO_CENTER 1500             // ~90 degrees
#define SERVO_MAX    2000             // ~180 degrees

/* Allowed angle range */
#define SERVO_ANGLE_MIN 0
#define SERVO_ANGLE_MAX 180

void test_moteur(void);                    // startup test: min -> max -> min
void servo_set_position(int16_t angle);    // clamped, left servo is mirrored
void servo_move(int16_t delta);            // relative move, clamped
int16_t servo_get_position(void);


#endif /* SOFTWARE_CORE_SERVO_SERVO_H_ */
