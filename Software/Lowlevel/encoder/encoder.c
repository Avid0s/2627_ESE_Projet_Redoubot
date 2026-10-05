/*
 * encoder.c
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */

#include "encoder.h"

typedef struct {
	TIM_HandleTypeDef *tim;
	bool     is32;           // true: 32-bit counter (TIM2), false: 16-bit (TIM4)
	bool     invert;
	uint32_t last;
	int32_t  total;
} encoder_t;

static encoder_t enc[2] = {
		[ENCODER_L] = { &ENCODER_L_TIM, true,  ENCODER_L_INVERT, 0, 0 },
		[ENCODER_R] = { &ENCODER_R_TIM, false, ENCODER_R_INVERT, 0, 0 },
};

void encoder_init(void){
	for (int i = 0; i < 2; i++) {
		HAL_TIM_Encoder_Start(enc[i].tim, TIM_CHANNEL_ALL);
		enc[i].last  = __HAL_TIM_GET_COUNTER(enc[i].tim);
		enc[i].total = 0;
	}
}

int32_t encoder_update(encoder_id_t e){
	encoder_t *p = &enc[e];
	uint32_t now = __HAL_TIM_GET_COUNTER(p->tim);
	int32_t delta;

	if (p->is32) delta = (int32_t)(now - p->last);
	else         delta = (int16_t)(uint16_t)(now - p->last);

	p->last = now;
	if (p->invert) delta = -delta;
	p->total += delta;
	return delta;
}

int32_t encoder_get_total(encoder_id_t e){
	return enc[e].total;
}

void encoder_reset(encoder_id_t e){
	enc[e].last  = __HAL_TIM_GET_COUNTER(enc[e].tim);
	enc[e].total = 0;
}
