/*
 * @file    APDS9960.c
 * @brief   Library for the Adafruit breackout board
 * @author  Mansard Johann
 * Created on: Oct 5, 2026
 * 
 * @copyright:  
 */

#include "APDS9960.h"
#include <stdio.h>

// Store the I2C handle globally for this file
static I2C_HandleTypeDef *apds_i2c;

// ====================================================================
// LOW LEVEL I2C ABSTRACTION (Replacing Arduino 'Wire' library)
// ====================================================================

static bool wireWriteDataByte(uint8_t reg, uint8_t val) {
	if (HAL_I2C_Mem_Write(apds_i2c, APDS9960_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 100) == HAL_OK) {
		return true;
	}
	return false;
}

static bool wireReadDataByte(uint8_t reg, uint8_t *val) {
	if (HAL_I2C_Mem_Read(apds_i2c, APDS9960_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, val, 1, 100) == HAL_OK) {
		return true;
	}
	return false;
}

// Used for reading 16-bit values (like colors)
static bool wireReadDataBlock(uint8_t reg, uint8_t *val, uint16_t len) {
	if (HAL_I2C_Mem_Read(apds_i2c, APDS9960_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, val, len, 100) == HAL_OK) {
		return true;
	}
	return false;
}

// ====================================================================
// INITIALIZATION
// ====================================================================

bool APDS9960_Init(I2C_HandleTypeDef *hi2c) {
	apds_i2c = hi2c;
	uint8_t id = 0;

	if (!wireReadDataByte(APDS9960_ID, &id)) return false;   // I2C failure
	if (id != 0xAB && id != 0x9C) return false;              // wrong chip

	// Start from a clean state (everything off)
	if (!wireWriteDataByte(APDS9960_ENABLE, 0x00)) return false;
	HAL_Delay(10);

	// ALS integration time: 0xDB = 103 ms
	if (!wireWriteDataByte(APDS9960_ATIME, 0xDB)) return false;
	// Proximity pulses: 8us length, 10 pulses
	if (!wireWriteDataByte(APDS9960_PPULSE, 0x89)) return false;
	// LED drive 100mA, proximity gain 4x, ALS gain 4x
	if (!wireWriteDataByte(APDS9960_CONTROL, 0x09)) return false;

	// Power on, then enable ALS + proximity
	if (!APDS9960_EnablePower()) return false;
	HAL_Delay(10);
	if (!APDS9960_EnableLightSensor(false)) return false;
	if (!APDS9960_EnableProximitySensor(false)) return false;

	HAL_Delay(120); // let first ALS integration finish
	return true;
}

// ====================================================================
// POWER & SENSOR CONTROL
// ====================================================================

bool APDS9960_EnablePower(void) {
	uint8_t val = 0;
	if (!wireReadDataByte(APDS9960_ENABLE, &val)) return false;
	val |= APDS9960_PON;
	return wireWriteDataByte(APDS9960_ENABLE, val);
}

bool APDS9960_DisablePower(void) {
	uint8_t val = 0;
	if (!wireReadDataByte(APDS9960_ENABLE, &val)) return false;
	val &= ~APDS9960_PON;
	return wireWriteDataByte(APDS9960_ENABLE, val);
}

bool APDS9960_EnableLightSensor(bool interrupts) {
	uint8_t val = 0;
	if (!wireReadDataByte(APDS9960_ENABLE, &val)) return false;

	val |= APDS9960_AEN; // Enable ALS
	if (interrupts) val |= APDS9960_AIEN; // Enable ALS Interrupts
	else val &= ~APDS9960_AIEN;

	return wireWriteDataByte(APDS9960_ENABLE, val);
}

bool APDS9960_DisableLightSensor(void) {
	uint8_t val = 0;
	if (!wireReadDataByte(APDS9960_ENABLE, &val)) return false;
	val &= ~(APDS9960_AEN | APDS9960_AIEN);
	return wireWriteDataByte(APDS9960_ENABLE, val);
}

bool APDS9960_EnableProximitySensor(bool interrupts) {
	uint8_t val = 0;
	if (!wireReadDataByte(APDS9960_ENABLE, &val)) return false;

	val |= APDS9960_PEN; // Enable Proximity
	if (interrupts) val |= APDS9960_PIEN;
	else val &= ~APDS9960_PIEN;

	return wireWriteDataByte(APDS9960_ENABLE, val);
}

bool APDS9960_DisableProximitySensor(void) {
	uint8_t val = 0;
	if (!wireReadDataByte(APDS9960_ENABLE, &val)) return false;
	val &= ~(APDS9960_PEN | APDS9960_PIEN);
	return wireWriteDataByte(APDS9960_ENABLE, val);
}

// ====================================================================
// DATA READING
// ====================================================================

bool APDS9960_ReadProximity(uint8_t *val) {
	return wireReadDataByte(APDS9960_PDATA, val);
}

bool APDS9960_ReadAmbientLight(uint16_t *val) {
	uint8_t buf[2];
	if (!wireReadDataBlock(APDS9960_CDATAL, buf, 2)) return false;
	*val = buf[0] | (buf[1] << 8);
	return true;
}

bool APDS9960_ReadRedLight(uint16_t *val) {
	uint8_t buf[2];
	if (!wireReadDataBlock(APDS9960_RDATAL, buf, 2)) return false;
	*val = buf[0] | (buf[1] << 8);
	return true;
}

bool APDS9960_ReadGreenLight(uint16_t *val) {
	uint8_t buf[2];
	if (!wireReadDataBlock(APDS9960_GDATAL, buf, 2)) return false;
	*val = buf[0] | (buf[1] << 8);
	return true;
}

bool APDS9960_ReadBlueLight(uint16_t *val) {
	uint8_t buf[2];
	if (!wireReadDataBlock(APDS9960_BDATAL, buf, 2)) return false;
	*val = buf[0] | (buf[1] << 8);
	return true;
}

// ====================================================================
// TEST
// ====================================================================

void APDS9960_Test(void) {
	uint8_t  prox = 0;
	uint16_t clear = 0, r = 0, g = 0, b = 0;

	bool ok = APDS9960_ReadProximity(&prox)
        				   && APDS9960_ReadAmbientLight(&clear)
						   && APDS9960_ReadRedLight(&r)
						   && APDS9960_ReadGreenLight(&g)
						   && APDS9960_ReadBlueLight(&b);

	if (!ok) {
		printf("APDS9960: I2C read error\r\n");
		return;
	}
	printf("Prox:%3u  Clear:%5u  R:%5u  G:%5u  B:%5u\r\n", prox, clear, r, g, b);
}
