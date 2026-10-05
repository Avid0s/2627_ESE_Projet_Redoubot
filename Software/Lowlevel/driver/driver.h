/*
 * driver.h
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_LOWLEVEL_DRIVER_H_
#define SOFTWARE_LOWLEVEL_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32g4xx_hal.h"

extern TIM_HandleTypeDef htim1;       // defined in main.c by CubeMX

#define DRIVER_TIM        htim1
#define DRIVER_L_CH_IN1   TIM_CHANNEL_1
#define DRIVER_L_CH_IN2   TIM_CHANNEL_2
#define DRIVER_R_CH_IN1   TIM_CHANNEL_3
#define DRIVER_R_CH_IN2   TIM_CHANNEL_4

typedef enum { DRIVER_L = 0, DRIVER_R = 1 } driver_id_t;

/* Duty is expressed in per-mille: 0 .. 1000 */
#define DRIVER_DUTY_MAX   1000

/* 1 = slow decay (IN1 held high, IN2 carries the PWM): more linear speed vs duty,
 *     a duty of 0 = brake.
 * 0 = fast decay (PWM on one input, the other low): a duty of 0 = coast. */
#define DRIVER_SLOW_DECAY 1

void driver_init(void);                              // starts PWM, wakes the drivers
void driver_sleep(bool sleep);                       // true = sleep (outputs high-Z)

void driver_forward(driver_id_t d, uint16_t duty);   // 0..1000
void driver_reverse(driver_id_t d, uint16_t duty);   // 0..1000
void driver_coast(driver_id_t d);                    // both inputs low (free-wheeling)
void driver_brake(driver_id_t d);                    // both inputs high (short the motor)

#endif /* SOFTWARE_LOWLEVEL_DRIVER_H_ */
