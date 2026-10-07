/*
 * tof_scan.h
 *
 *  Created on: 7 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_MIDDLEWARE_TOF_SCAN_TOF_SCAN_H_
#define SOFTWARE_MIDDLEWARE_TOF_SCAN_TOF_SCAN_H_

#include <stdint.h>
#include <math.h>
#include "tof.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float   angle_deg;   // angle since the scan started (0 .. sweep), sensor axis
	int16_t dist_mm;     // measured distance, -1 = invalid/timeout, >=TOF_NO_TARGET_MM = nothing seen
	uint32_t t_ms;       // HAL_GetTick() at the end of the measurement
} ScanPoint;

typedef struct {
	float angle_deg;     // bearing of the object's centre (relative to scan start)
	float dist_mm;       // distance to the closest part of the object
	float width_mm;      // approximate chord width (from angular extent at mean distance)
	float x_mm, y_mm;    // centre in robot frame: x = forward at angle 0, y = left (CCW positive)
	int   n_points;
} ScanObject;

typedef float (*HeadingFn)(void);

//
// Take up to maxPts samples until the robot has turned sweep_deg or timeout_ms elapsed.
// The caller is responsible for turning the robot. Returns the number of points stored.
// The heading is read before and after each measurement and averaged, which removes
// most of the smear caused by the ~20-33 ms measurement time.
//
int scanRun(ScanPoint *pts, int maxPts, float sweep_deg, HeadingFn heading, uint32_t timeout_ms);

//
// Split a scan into objects.
//   max_range_mm : ignore everything farther than this (walls, far field)
//   jump_mm      : a change larger than this between neighbouring samples starts a new object
//   min_pts      : discard segments with fewer samples (noise)
// Returns the number of objects written to out (max maxObj).
//
int scanFindObjects(const ScanPoint *pts, int n, ScanObject *out, int maxObj,
		int max_range_mm, int jump_mm, int min_pts);

#ifdef __cplusplus
}
#endif

#endif /* SOFTWARE_MIDDLEWARE_TOF_SCAN_TOF_SCAN_H_ */
