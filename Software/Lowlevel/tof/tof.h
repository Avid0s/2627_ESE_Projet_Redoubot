/*
 * tof.h
 *
 *  Created on: 7 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_LOWLEVEL_TOF_TOF_H_
#define SOFTWARE_LOWLEVEL_TOF_TOF_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

// Distances >= this value mean "no target in range" (the chip reports ~8190)
#define TOF_NO_TARGET_MM 8000

//
// Reset the sensor (XSHUT_TOF pin, if defined by CubeMX), then initialise it.
//   hi2c       : e.g. &hi2c3
//   bLongRange : 0 = ~30..800 mm, 1 = up to ~2000 mm (more sensitive to ambient light)
// Returns 1 on success, 0 on failure (no ACK on 0x29 or an I2C error).
//
int tofInit(I2C_HandleTypeDef *hi2c, int bLongRange);

//
// Blocking single measurement (~33 ms with the default timing budget).
// Returns the distance in mm, or -1 on timeout / I2C error.
// Call from the main loop, never from an interrupt handler.
//
int tofReadDistance(void);

// Model / revision register. The VL53L0X answers model = 0xEE.
int tofGetModel(int *model, int *revision);

// Measurement time per sample in us (min 20000, default ~33000). Call after tofInit.
int tofSetTimingBudget(uint32_t budget_us);

// Non-zero if the last transfer failed
int tofGetError(void);
void tofClearError(void);

#ifdef __cplusplus
}

#endif

#endif /* SOFTWARE_LOWLEVEL_TOF_TOF_H_ */
