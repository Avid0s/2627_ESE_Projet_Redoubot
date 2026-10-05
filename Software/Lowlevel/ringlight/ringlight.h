/*
 * ringlight.h
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_LOWLEVEL_NEOPIXEL_RINGLIGHT_H_
#define SOFTWARE_LOWLEVEL_NEOPIXEL_RINGLIGHT_H_

#include <stdbool.h>
#include <stdint.h>
#include "neopixel.h"

/* Light color and brightness (0..255). White is the neutral choice for a red/blue
 * measurement. Keep the same values between calibration and use. */
#define RING_R            255
#define RING_G            255
#define RING_B            255
#define RING_BRIGHTNESS   128

/* Time after switch-on before the light is considered stable for a measurement */
#define RING_SETTLE_MS     50

void ring_light_init(void);                  // ring off
void ring_light_set(bool on);                // e.g. ring_light_set(object_present)
void ring_light_update(void);                // call at every main loop iteration
bool ring_light_is_on(void);
bool ring_light_is_ready(void);              // lit and settled: the color sensor can be read

#endif /* SOFTWARE_LOWLEVEL_NEOPIXEL_RINGLIGHT_H_ */
