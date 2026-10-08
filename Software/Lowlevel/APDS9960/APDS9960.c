/*
 * @file    APDS9960.c
 * @brief   Library for the Adafruit breackout board
 * @author  Mansard Johann
 * Created on: Oct 5, 2026
 * 
 * @copyright:  
 */

#include "APDS9960.h"

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

    // 1. Check Sensor ID to confirm communication
    if (!wireReadDataByte(APDS9960_ID, &id)) {
        return false; // I2C communication failed
    }

    // SparkFun APDS9960 normally returns 0xAB. Some variants return 0x9C.
    if (id != 0xAB && id != 0x9C) {
        return false; // Wrong ID
    }

    // 2. Set Default Settings (Power On)
    if (!APDS9960_EnablePower()) {
        return false;
    }

    HAL_Delay(10); // Give sensor time to power up
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

bool APDS9960_EnableProximitySensor(bool interrupts) {
    uint8_t val = 0;
    if (!wireReadDataByte(APDS9960_ENABLE, &val)) return false;

    val |= APDS9960_PEN; // Enable Proximity
    if (interrupts) val |= APDS9960_PIEN;
    else val &= ~APDS9960_PIEN;

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
//uint8_t APDS9960_init(){
//    uint8_t ret;
//    if (i2c_read_data()!=APDS9960_ID){
//        goto cleanup;
//    }
//    if (ret<0) goto cleanup;
//cleanup:
//    return ret;
//}
