/*
 * gripper.c
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#include "gripper.h"

#define GRIP_DEBOUNCE_MS  200        // object must be seen this long before grabbing
#define GRIP_CLOSE_MS     500        // time allowed for the jaws to close

typedef enum { GRIP_OPEN, GRIP_CLOSING, GRIP_HOLDING } grip_state_t;

static grip_state_t grip_state = GRIP_OPEN;
static uint32_t grip_t0;

void gripper_open(void){
	servo_set_position(GRIPPER_OPEN);
	grip_state = GRIP_OPEN;
	grip_t0 = HAL_GetTick();
}

// Call at every main loop iteration with the current sensor state
void gripper_update(bool object_present){
	switch (grip_state) {
	case GRIP_OPEN:
		if (!object_present) {
			grip_t0 = HAL_GetTick();                         // restart debounce timer
		} else if (HAL_GetTick() - grip_t0 >= GRIP_DEBOUNCE_MS) {
			servo_set_position(GRIPPER_CLOSED);
			grip_t0 = HAL_GetTick();
			grip_state = GRIP_CLOSING;
		}
		break;

	case GRIP_CLOSING:
		if (HAL_GetTick() - grip_t0 >= GRIP_CLOSE_MS)
			grip_state = GRIP_HOLDING;
		break;

	case GRIP_HOLDING:
		break;                                               // stays closed until gripper_open()
	}
}

bool gripper_has_object(void){
	return grip_state == GRIP_HOLDING;
}
