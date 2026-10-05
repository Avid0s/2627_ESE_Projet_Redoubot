/*
 * battery_led.c
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#include "battery_led.h"

static uint8_t  level = 100;
static bool     level_valid = false;     // LED stays off until the first level is received
static bool     blinking = false;
static bool     blink_on = true;
static uint32_t blink_t0;

// Last color pushed to the LED, to avoid useless transfers
static uint8_t  last_r, last_g, last_b;
static bool     last_valid = false;

// 100 % -> green, 50 % -> yellow, 0 % -> red
static void level_to_color(uint8_t percent, uint8_t *r, uint8_t *g, uint8_t *b){
	if (percent > 100) percent = 100;

	if (percent >= 50) {
		*r = (uint8_t)(((100 - percent) * 2 * 255) / 100);
		*g = 255;
	} else {
		*r = 255;
		*g = (uint8_t)((percent * 2 * 255) / 100);
	}
	*b = 0;
}

void battery_led_init(void){
	last_valid = false;
}

void battery_led_set_level(uint8_t percent){
	level = (percent > 100) ? 100 : percent;
	level_valid = true;
}

bool battery_led_is_blinking(void){
	return blinking;
}

void battery_led_update(void){
	uint32_t now = HAL_GetTick();

	if (!level_valid) return;

	/* Blink state with hysteresis */
	if (!blinking && level <= BATT_LED_LOW_PERCENT) {
		blinking = true;
		blink_on = true;
		blink_t0 = now;
	} else if (blinking && level > BATT_LED_LOW_PERCENT + BATT_LED_HYSTERESIS) {
		blinking = false;
	}

	if (blinking && (now - blink_t0 >= BATT_LED_BLINK_MS)) {
		blink_on = !blink_on;
		blink_t0 = now;
	}

	/* Target color */
	uint8_t r = 0, g = 0, b = 0;
	if (!blinking || blink_on)
		level_to_color(level, &r, &g, &b);

	r = (uint8_t)((r * BATT_LED_BRIGHTNESS) / 255);
	g = (uint8_t)((g * BATT_LED_BRIGHTNESS) / 255);
	b = (uint8_t)((b * BATT_LED_BRIGHTNESS) / 255);

	/* Push only on change; if the DMA is busy, we retry at the next call */
	if (last_valid && r == last_r && g == last_g && b == last_b) return;

	neopixel_set(NEOPIXEL_BATT_INDEX, r, g, b);
	if (neopixel_show()) {
		last_r = r; last_g = g; last_b = b;
		last_valid = true;
	}
}
