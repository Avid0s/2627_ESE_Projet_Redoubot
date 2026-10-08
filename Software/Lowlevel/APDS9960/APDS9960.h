/*
 * APDS9960.h
 *
 *  Created on: Oct 5, 2026
 *      Author: johann
 */

#ifndef STM32CUBEMX_LOWLEVEL_APDS9960_APDS9960_H_
#define STM32CUBEMX_LOWLEVEL_APDS9960_APDS9960_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <stm32g4xx.h>

#define APDS9960_I2C_ADDR		(0x39<<1)

/*----------------------------------------
 * ADPS-9960 Register Addresses
 -----------------------------------------*/

#define APDS9960_RAM_START 		0x00  // R/W RAM
#define APDS9960_RAM_END 		0x7F  // R/W RAM
#define APDS9960_ENABLE 		0x80  // R/W Enable states and interrupts
#define APDS9960_ATIME 			0x81  // R/W ADC integration time
#define APDS9960_WTIME 			0x83  // R/W Wait time (non-gesture)
#define APDS9960_AILTL 			0x84  // R/W ALS interrupt low threshold low byte --
#define APDS9960_AILTH 			0x85  // R/W ALS interrupt low threshold high byte --
#define APDS9960_AIHTL 			0x86  // R/W ALS interrupt high threshold low byte 0x00
#define APDS9960_AIHTH 			0x87  // R/W ALS interrupt high threshold high byte 0x00
#define APDS9960_PILT 			0x89  // R/W Proximity interrupt low threshold 0x00
#define APDS9960_PIHT 			0x8B  // R/W Proximity interrupt high threshold 0x00
#define APDS9960_PERS 			0x8C  // R/W Interrupt persistence filters (non-gesture) 0x00
#define APDS9960_CONFIG1 		0x8D  // R/W Configuration register one 0x40
#define APDS9960_PPULSE 		0x8E  // R/W Proximity pulse count and length 0x40
#define APDS9960_CONTROL 		0x8F  // R/W Gain control 0x00
#define APDS9960_CONFIG2 		0x90  // R/W Configuration register two 0x01
#define APDS9960_ID 			0x92  // R Device ID ID
#define APDS9960_STATUS 		0x93  // R Device status 0x00
#define APDS9960_CDATAL 		0x94  // R Low byte of clear channel data 0x00
#define APDS9960_CDATAH 		0x95  // R High byte of clear channel data 0x00
#define APDS9960_RDATAL 		0x96  // R Low byte of red channel data 0x00
#define APDS9960_RDATAH 		0x97  // R High byte of red channel data 0x00
#define APDS9960_GDATAL 		0x98  // R Low byte of green channel data 0x00
#define APDS9960_GDATAH 		0x99  // R High byte of green channel data 0x00
#define APDS9960_BDATAL 		0x9A  // R Low byte of blue channel data 0x00
#define APDS9960_BDATAH 		0x9B  // R High byte of blue channel data 0x00
#define APDS9960_PDATA 			0x9C  // R Proximity data 0x00
#define APDS9960_POFFSET_UR 	0x9D  // R/W Proximity offset for UP and RIGHT photodiodes 0x00
#define APDS9960_POFFSET_DL 	0x9E  // R/W Proximity offset for DOWN and LEFT photodiodes 0x00
#define APDS9960_CONFIG3 		0x9F  // R/W Configuration register three 0x00
#define APDS9960_GPENTH 		0xA0  // R/W Gesture proximity enter threshold 0x00
#define APDS9960_GEXTH 			0xA1  // R/W Gesture exit threshold 0x00
#define APDS9960_GCONF1 		0xA2  // R/W Gesture configuration one 0x00
#define APDS9960_GCONF2 		0xA3  // R/W Gesture configuration two 0x00
#define APDS9960_GOFFSET_U 		0xA4  // R/W Gesture UP offset register 0x00
#define APDS9960_GOFFSET_D 		0xA5  // R/W Gesture DOWN offset register 0x00
#define APDS9960_GOFFSET_L 		0xA7  // R/W Gesture LEFT offset register 0x00
#define APDS9960_GOFFSET_R 		0xA9  // R/W Gesture RIGHT offset register 0x00
#define APDS9960_GPULSE 		0xA6  // R/W Gesture pulse count and length 0x40
#define APDS9960_GCONF3 		0xAA  // R/W Gesture configuration three 0x00
#define APDS9960_GCONF4 		0xAB  // R/W Gesture configuration four 0x00
#define APDS9960_GFLVL 			0xAE  // R Gesture FIFO level 0x00
#define APDS9960_GSTATUS 		0xAF  // R Gesture status 0x00
#define APDS9960_IFORCE 		0xE4  // W Force interrupt 0x00
#define APDS9960_PICLEAR 		0xE5  // W Proximity interrupt clear 0x00
#define APDS9960_CICLEAR 		0xE6  // W ALS clear channel interrupt clear 0x00
#define APDS9960_AICLEAR 		0xE7  // W All non-gesture interrupts clear 0x00
#define APDS9960_GFIFO_U 		0xFC  // R Gesture FIFO UP value 0x00
#define APDS9960_GFIFO_D 		0xFD  // R Gesture FIFO DOWN value 0x00
#define APDS9960_GFIFO_L 		0xFE  // R Gesture FIFO LEFT value 0x00
#define APDS9960_GFIFO_R 		0xFF  // R Gesture FIFO RIGHT value 0x00

/* ENABLE Register Bits */
#define APDS9960_PON            0x01
#define APDS9960_AEN            0x02
#define APDS9960_PEN            0x04
#define APDS9960_WEN            0x08
#define APDS9960_AIEN           0x10
#define APDS9960_PIEN           0x20
#define APDS9960_GEN            0x40

/*----------------------------------------
 * Function Prototypes
 -----------------------------------------*/
bool APDS9960_Init(I2C_HandleTypeDef *hi2c);

/* Enable / Disable Features */
bool APDS9960_EnablePower(void);
bool APDS9960_DisablePower(void);
bool APDS9960_EnableLightSensor(bool interrupts);
bool APDS9960_DisableLightSensor(void);
bool APDS9960_EnableProximitySensor(bool interrupts);
bool APDS9960_DisableProximitySensor(void);

/* Read Data */
bool APDS9960_ReadProximity(uint8_t *val);
bool APDS9960_ReadAmbientLight(uint16_t *val);
bool APDS9960_ReadRedLight(uint16_t *val);
bool APDS9960_ReadGreenLight(uint16_t *val);
bool APDS9960_ReadBlueLight(uint16_t *val);

#endif /* STM32CUBEMX_LOWLEVEL_APDS9960_APDS9960_H_ */
