/*
 * tof_scan.c
 *
 *  Created on: 7 oct. 2026
 *      Author: geoff
 */
#include "tof_scan.h"

#define DEG2RAD 0.01745329252f

// wrap an angle difference to (-180, 180]
static float wrap180(float a)
{
	while (a > 180.0f)  a -= 360.0f;
	while (a <= -180.0f) a += 360.0f;
	return a;
}

int scanRun(ScanPoint *pts, int maxPts, float sweep_deg, HeadingFn heading, uint32_t timeout_ms)
{
	int n = 0;
	uint32_t t0 = HAL_GetTick();
	float prev = heading();
	float rel = 0.0f;                 // unwrapped angle since start

	while (n < maxPts && (HAL_GetTick() - t0) < timeout_ms) {
		float ha = heading();
		int d = tofReadDistance();
		float hb = heading();

		float hmid = ha + wrap180(hb - ha) * 0.5f;      // heading in the middle of the measurement
		rel += wrap180(hmid - prev);
		prev = hmid;

		if (fabsf(rel) > sweep_deg) break;
		pts[n].angle_deg = rel;
		pts[n].dist_mm   = (int16_t)(d > 32000 ? 32000 : d);
		pts[n].t_ms      = HAL_GetTick();
		n++;
	}
	return n;
}

static int valid(const ScanPoint *p, int max_range_mm)
{
	return p->dist_mm > 0 && p->dist_mm < TOF_NO_TARGET_MM && p->dist_mm <= max_range_mm;
}

static int finish(const ScanPoint *pts, int a, int b, ScanObject *o)
{
	// segment = samples a..b inclusive
	float sumA = 0, sumD = 0, minD = 1e9f;
	int cnt = b - a + 1;
	for (int i = a; i <= b; i++) {
		sumA += pts[i].angle_deg;
		sumD += pts[i].dist_mm;
		if (pts[i].dist_mm < minD) minD = pts[i].dist_mm;
	}
	float meanA = sumA / cnt, meanD = sumD / cnt;
	float extent = fabsf(pts[b].angle_deg - pts[a].angle_deg);
	o->angle_deg = meanA;
	o->dist_mm   = minD;
	o->width_mm  = 2.0f * meanD * tanf(0.5f * extent * DEG2RAD);
	o->x_mm      = meanD * cosf(meanA * DEG2RAD);
	o->y_mm      = meanD * sinf(meanA * DEG2RAD);
	o->n_points  = cnt;
	return 1;
}

int scanFindObjects(const ScanPoint *pts, int n, ScanObject *out, int maxObj,
		int max_range_mm, int jump_mm, int min_pts)
{
	int k = 0, start = -1;
	for (int i = 0; i <= n && k < maxObj; i++) {
		int ok = (i < n) && valid(&pts[i], max_range_mm);
		int brk = 0;
		if (ok && start >= 0) {
			int dd = pts[i].dist_mm - pts[i - 1].dist_mm;
			if (dd < 0) dd = -dd;
			brk = (dd > jump_mm);
		}
		if (start >= 0 && (!ok || brk)) {
			if (i - start >= min_pts) k += finish(pts, start, i - 1, &out[k]);
			start = -1;
		}
		if (ok && start < 0) start = i;
	}
	return k;
}
