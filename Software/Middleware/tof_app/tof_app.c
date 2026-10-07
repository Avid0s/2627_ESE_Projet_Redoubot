/*
 * tof_app.c
 *
 *  Created on: 7 oct. 2026
 *      Author: geoff
 */

#include "tof_app.h"

extern I2C_HandleTypeDef  hi2c3;    // ToF bus (PC8 / PC9)
extern UART_HandleTypeDef huart1;   // ST-Link virtual COM port

// ---- settings ----------------------------------------------------------
#define TOF_LONG_RANGE     0        // 0 = ~30..800 mm, 1 = up to ~2000 mm
#define TOF_BUDGET_US      20000    // 20 ms = ~45-50 samples/s (default ~33 ms)
#define TOF_PRINT_PERIOD   100      // ms between debug prints

#define SCAN_SWEEP_DEG     360.0f   // how far to turn per scan
#define SCAN_TIMEOUT_MS    10000    // give up after this long
#define SCAN_MAX_PTS       200      // buffer size (~2.4 kB)
#define OBJ_MAX_RANGE_MM   600      // ignore anything farther (walls, far field)
#define OBJ_JUMP_MM        80       // distance step that separates two objects
#define OBJ_MIN_PTS        2        // fewer samples than this = noise

static int tof_ok = 0;
static TofRobotHooks robot = {0};
static ScanPoint scanBuf[SCAN_MAX_PTS];

static void log_msg(const char *s)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)s, strlen(s), 100);
}

// ---- setup -------------------------------------------------------------
int tof_app_init(void)
{
	char buf[64];
	int model = 0, rev = 0;

	if (!tofInit(&hi2c3, TOF_LONG_RANGE)) {
		log_msg("ToF: init FAILED (check wiring / pull-ups / XSHUT)\r\n");
		tof_ok = 0;
		return 0;
	}
	tofGetModel(&model, &rev);
	snprintf(buf, sizeof(buf), "ToF: model 0x%02X rev 0x%02X (expect 0xEE)\r\n", model, rev);
	log_msg(buf);

	tofSetTimingBudget(TOF_BUDGET_US);
	tof_ok = 1;
	return 1;
}

void tof_app_set_robot(const TofRobotHooks *h)
{
	if (h) robot = *h;
}

// ---- everyday use ------------------------------------------------------
int tof_app_distance(void)
{
	return tof_ok ? tofReadDistance() : -1;
}

int tof_app_scan(ScanObject *objs, int maxObj)
{
	int n, k;

	if (!tof_ok || !robot.start_turn || !robot.stop_turn || !robot.heading_deg)
		return -1;

	robot.start_turn();
	n = scanRun(scanBuf, SCAN_MAX_PTS, SCAN_SWEEP_DEG, robot.heading_deg, SCAN_TIMEOUT_MS);
	robot.stop_turn();

	k = scanFindObjects(scanBuf, n, objs, maxObj, OBJ_MAX_RANGE_MM, OBJ_JUMP_MM, OBJ_MIN_PTS);

	// sort nearest first (insertion sort, k is tiny)
	for (int i = 1; i < k; i++) {
		ScanObject t = objs[i];
		int j = i - 1;
		while (j >= 0 && objs[j].dist_mm > t.dist_mm) { objs[j + 1] = objs[j]; j--; }
		objs[j + 1] = t;
	}
	return k;
}

int tof_app_nearest(ScanObject *obj)
{
	ScanObject list[8];
	int k = tof_app_scan(list, 8);
	if (k <= 0) return 0;
	*obj = list[0];
	return 1;
}

// ---- debugging ---------------------------------------------------------
void tof_app_run(void)
{
	static uint32_t last = 0;
	char buf[48];
	int d;

	if (!tof_ok) return;
	if (HAL_GetTick() - last < TOF_PRINT_PERIOD) return;
	last = HAL_GetTick();

	d = tofReadDistance();
	if (d < 0)                       snprintf(buf, sizeof(buf), "ToF: error\r\n");
	else if (d >= TOF_NO_TARGET_MM)  snprintf(buf, sizeof(buf), "ToF: no target\r\n");
	else                             snprintf(buf, sizeof(buf), "ToF: %d mm\r\n", d);
	log_msg(buf);
}
