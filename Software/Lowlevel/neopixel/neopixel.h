/*
 * neopixel.h
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_LOWLEVEL_NEOPIXEL_NEOPIXEL_H_
#define SOFTWARE_LOWLEVEL_NEOPIXEL_NEOPIXEL_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32g4xx_hal.h"

extern TIM_HandleTypeDef htim3;       // defined in main.c by CubeMX

#define NEOPIXEL_TIM                  htim3
#define NEOPIXEL_CH                   TIM_CHANNEL_1

/* Number of LEDs on the data line (to be set to match the real hardware) */
#define NEOPIXEL_COUNT                1

/* Global brightness 0..255, keeps the current draw reasonable */
#define NEOPIXEL_BRIGHTNESS_DEFAULT   64

void neopixel_init(void);                                  // clears all LEDs
void neopixel_set(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void neopixel_fill(uint8_t r, uint8_t g, uint8_t b);
void neopixel_clear(void);
void neopixel_set_brightness(uint8_t brightness);          // 0..255
bool neopixel_show(void);                                  // non-blocking, false if a transfer is still running
bool neopixel_is_busy(void);

#endif /* SOFTWARE_LOWLEVEL_NEOPIXEL_NEOPIXEL_H_ */
