/*
 * neopixel.c
 *
 *  Created on: 5 oct. 2026
 *      Author: geoff
 */


#include "neopixel.h"

#define RESET_SLOTS   80                                   // 80 x 1.25 us = 100 us of low level
#define BUF_LEN       (NEOPIXEL_COUNT * 24 + RESET_SLOTS)

static uint8_t  colors[NEOPIXEL_COUNT][3];                 // r, g, b
static uint16_t buf[BUF_LEN];                              // one compare value per bit
static uint8_t  brightness = NEOPIXEL_BRIGHTNESS_DEFAULT;
static volatile bool busy = false;

void neopixel_init(void){
	neopixel_clear();
	neopixel_show();
}

void neopixel_set(uint16_t index, uint8_t r, uint8_t g, uint8_t b){
	if (index >= NEOPIXEL_COUNT) return;
	colors[index][0] = r;
	colors[index][1] = g;
	colors[index][2] = b;
}

void neopixel_fill(uint8_t r, uint8_t g, uint8_t b){
	for (uint16_t i = 0; i < NEOPIXEL_COUNT; i++) neopixel_set(i, r, g, b);
}

void neopixel_clear(void){
	neopixel_fill(0, 0, 0);
}

void neopixel_set_brightness(uint8_t b){
	brightness = b;
}

bool neopixel_is_busy(void){
	return busy;
}

// Encodes the colors into the DMA buffer and starts the transfer
bool neopixel_show(void){
	if (busy) return false;

	// Bit durations from the actual timer period: 0 -> 32 % high (~0.4 us), 1 -> 64 % high (~0.8 us)
	uint32_t period = __HAL_TIM_GET_AUTORELOAD(&NEOPIXEL_TIM) + 1;
	uint16_t t0 = (uint16_t)(period * 32 / 100);
	uint16_t t1 = (uint16_t)(period * 64 / 100);

	uint32_t k = 0;
	for (uint16_t i = 0; i < NEOPIXEL_COUNT; i++) {
		uint32_t r = ((uint32_t)colors[i][0] * brightness) / 255;
		uint32_t g = ((uint32_t)colors[i][1] * brightness) / 255;
		uint32_t b = ((uint32_t)colors[i][2] * brightness) / 255;
		uint32_t grb = (g << 16) | (r << 8) | b;           // WS2812 expects G, R, B, MSB first

		for (int bit = 23; bit >= 0; bit--)
			buf[k++] = ((grb >> bit) & 1) ? t1 : t0;
	}
	while (k < BUF_LEN) buf[k++] = 0;                      // reset / latch

	busy = true;
	if (HAL_TIM_PWM_Start_DMA(&NEOPIXEL_TIM, NEOPIXEL_CH, (uint32_t *)buf, BUF_LEN) != HAL_OK) {
		busy = false;
		return false;
	}
	return true;
}

// Called by the HAL when the DMA transfer is complete
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == NEOPIXEL_TIM.Instance) {
		HAL_TIM_PWM_Stop_DMA(htim, NEOPIXEL_CH);
		busy = false;
	}
}
