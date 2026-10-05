/*
 * ringlight.c
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#include "ringlight.h"

static bool     target_on = false;
static bool     pending = false;     // a change still has to be pushed to the LEDs
static uint32_t t_on;

void ring_light_init(void){
	target_on = false;
	pending = true;                  // push the "off" state at the first update
}

void ring_light_set(bool on){
	if (on == target_on) return;
	target_on = on;
	pending = true;
	if (on) t_on = HAL_GetTick();
}

void ring_light_update(void){
	if (!pending) return;

	uint8_t r = 0, g = 0, b = 0;
	if (target_on) {
		r = (uint8_t)((RING_R * RING_BRIGHTNESS) / 255);
		g = (uint8_t)((RING_G * RING_BRIGHTNESS) / 255);
		b = (uint8_t)((RING_B * RING_BRIGHTNESS) / 255);
	}

	neopixel_fill_range(NEOPIXEL_RING_FIRST, NEOPIXEL_RING_COUNT, r, g, b);
	if (neopixel_show())             // if the DMA is busy, we retry at the next call
		pending = false;
}

bool ring_light_is_on(void){
	return target_on;
}

bool ring_light_is_ready(void){
	return target_on && !pending && (HAL_GetTick() - t_on >= RING_SETTLE_MS);
}
