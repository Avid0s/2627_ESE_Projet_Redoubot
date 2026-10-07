/*
 * tof_app.h
 *
 *  Created on: 7 oct. 2026
 *      Author: geoff
 */

#ifndef SOFTWARE_MIDDLEWARE_TOF_APP_TOF_APP_H_
#define SOFTWARE_MIDDLEWARE_TOF_APP_TOF_APP_H_

#include <stdint.h>
#include "tof_scan.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "tof.h"

// Robot functions the scan needs. Fill in once, at start-up.
typedef struct {
	void  (*start_turn)(void);     // begin turning on the spot at a slow, constant speed
	void  (*stop_turn)(void);      // stop the motors
	float (*heading_deg)(void);    // current heading in degrees (encoders or MPU-6050 gyro)
} TofRobotHooks;

// ---- setup -------------------------------------------------------------
int  tof_app_init(void);                          // call once; 1 = sensor OK
void tof_app_set_robot(const TofRobotHooks *h);   // give the scan access to your motors / heading

// ---- everyday use ------------------------------------------------------
int  tof_app_distance(void);                      // one reading in mm, -1 error, >=8000 nothing seen
int  tof_app_scan(ScanObject *objs, int maxObj);  // full 360° turn + scan; objects sorted nearest first.
//   returns object count, -1 if hooks not set / sensor down
int  tof_app_nearest(ScanObject *obj);            // scan and return only the closest object (1 = found)

// ---- debugging ---------------------------------------------------------
void tof_app_run(void);                           // call every loop: prints distance on USART1 every 100 ms

#endif /* SOFTWARE_MIDDLEWARE_TOF_APP_TOF_APP_H_ */
