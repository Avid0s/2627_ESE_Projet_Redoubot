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

#define NEOPIXEL_TIM            htim3
#define NEOPIXEL_CH             TIM_CHANNEL_1

/* Chain layout (set NEOPIXEL_RING_COUNT to match the real ring) */
#define NEOPIXEL_BATT_INDEX     0
#define NEOPIXEL_RING_FIRST     1
#define NEOPIXEL_RING_COUNT     12
#define NEOPIXEL_COUNT          (NEOPIXEL_RING_FIRST + NEOPIXEL_RING_COUNT)

void neopixel_init(void);                                  // clears all LEDs
void neopixel_set(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void neopixel_fill_range(uint16_t first, uint16_t count, uint8_t r, uint8_t g, uint8_t b);
void neopixel_clear(void);
bool neopixel_show(void);                                  // non-blocking, false if a transfer is still running
bool neopixel_is_busy(void);

#endif /* SOFTWARE_LOWLEVEL_NEOPIXEL_NEOPIXEL_H_ */
