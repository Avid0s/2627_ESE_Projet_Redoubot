/*
 * battery_led.h
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_MIDDLEWARE_BATTERY_LED_BATTERY_LED_H_
#define SOFTWARE_MIDDLEWARE_BATTERY_LED_BATTERY_LED_H_

#include <stdbool.h>
#include <stdint.h>
#include "neopixel.h"

/* Brightness of the battery pixel, 0..255 (the ring has its own setting) */
#define BATT_LED_BRIGHTNESS     64

/* Below this level (in %) the LED blinks */
#define BATT_LED_LOW_PERCENT    20
/* The blinking stops only above LOW + HYSTERESIS, to avoid flickering around the threshold */
#define BATT_LED_HYSTERESIS      3
/* Blink half period: LED on for this long, then off for this long */
#define BATT_LED_BLINK_MS      500

void battery_led_init(void);
void battery_led_set_level(uint8_t percent);     // 0..100, from the fuel gauge
void battery_led_update(void);                   // call at every main loop iteration
bool battery_led_is_blinking(void);

#endif /* SOFTWARE_MIDDLEWARE_BATTERY_LED_BATTERY_LED_H_ */
