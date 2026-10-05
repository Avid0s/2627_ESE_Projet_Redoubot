/*
 * encoder.h
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_LOWLEVEL_ENCODER_H_
#define SOFTWARE_LOWLEVEL_ENCODER_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32g4xx_hal.h"

extern TIM_HandleTypeDef htim2;       // defined in main.c by CubeMX
extern TIM_HandleTypeDef htim4;

#define ENCODER_L_TIM     htim2
#define ENCODER_R_TIM     htim4

typedef enum { ENCODER_L = 0, ENCODER_R = 1 } encoder_id_t;

/* Counts per output shaft revolution: 7 pulses x 4 (TI12) x 100 (gearbox) */
#define ENCODER_CPR       2800

/* Flip so that a positive count means "forward" (motors are mirrored) */
#define ENCODER_L_INVERT  0
#define ENCODER_R_INVERT  1

void    encoder_init(void);                       // starts both timers
int32_t encoder_update(encoder_id_t e);           // counts since last call (wraparound-safe), call at a fixed rate
int32_t encoder_get_total(encoder_id_t e);        // accumulated counts as of last update
void    encoder_reset(encoder_id_t e);            // total = 0

#endif /* SOFTWARE_LOWLEVEL_ENCODER_H_ */
