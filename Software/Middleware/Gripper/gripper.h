/*
 * gripper.h
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_MIDDLEWARE_GRIPPER_GRIPPER_H_
#define SOFTWARE_MIDDLEWARE_GRIPPER_GRIPPER_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "servo.h"
#include "main.h"

/* Gripper positions: to be calibrated on the real mechanism.
 * Do not command a fully closed position when gripping an object,
 * otherwise the servos stall against it (heat, current, buzzing). */
#define GRIPPER_OPEN    120
#define GRIPPER_CLOSED   40

void gripper_open(void);
void gripper_update(bool object_present);  // call at every main loop iteration
bool gripper_has_object(void);


#endif /* SOFTWARE_MIDDLEWARE_GRIPPER_GRIPPER_H_ */
