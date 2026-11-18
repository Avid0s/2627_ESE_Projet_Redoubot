#include "Fuel_Gauge.h"

#define BQ27220_I2C_TIMEOUT_MS 50U

// Lecture générique d'un mot de 16 bits (Little Endian sur le BQ27220)
static HAL_StatusTypeDef BQ27220_ReadWord(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t *value) {
    uint8_t buf[2] = {0};
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, BQ27220_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buf, 2, BQ27220_I2C_TIMEOUT_MS);
    
    if (status == HAL_OK) {
        *value = (uint16_t)((buf[1] << 8) | buf[0]);
    }
    return status;
}

// Vérifie que le composant répond bien sur le bus I2C (test crucial pour le boîtier WCSP)
HAL_StatusTypeDef BQ27220_Init(I2C_HandleTypeDef *hi2c, BQ27220_Data_t *data) {
    data->is_present = false;
    
    // Ping I2C (3 essais)
    if (HAL_I2C_IsDeviceReady(hi2c, BQ27220_I2C_ADDR, 3, BQ27220_I2C_TIMEOUT_MS) != HAL_OK) {
        return HAL_ERROR;
    }

    uint16_t dev_id = 0;
    if (BQ27220_GetDeviceID(hi2c, &dev_id) == HAL_OK) {
        if (dev_id == 0x0220U) {
            data->is_present = true;
            return BQ27220_Update(hi2c, data);
        }
    }
    return HAL_ERROR;
}

// Lecture du Device ID via la sous-commande Control()
HAL_StatusTypeDef BQ27220_GetDeviceID(I2C_HandleTypeDef *hi2c, uint16_t *device_id) {
    uint8_t cmd[2] = { (uint8_t)(BQ27220_SUBCMD_DEVICE_TYPE & 0xFF), (uint8_t)(BQ27220_SUBCMD_DEVICE_TYPE >> 8) };
    
    if (HAL_I2C_Mem_Write(hi2c, BQ27220_I2C_ADDR, BQ27220_REG_CONTROL, I2C_MEMADD_SIZE_8BIT, cmd, 2, BQ27220_I2C_TIMEOUT_MS) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Petit délai nécessaire pour que la jauge place le résultat dans le registre Control
    HAL_Delay(2); 
    
    return BQ27220_ReadWord(hi2c, BQ27220_REG_CONTROL, device_id);
}

// Met à jour toutes les mesures principales en une seule passe
HAL_StatusTypeDef BQ27220_Update(I2C_HandleTypeDef *hi2c, BQ27220_Data_t *data) {
    uint16_t raw_val = 0;

    // 1. Tension (mV)
    if (BQ27220_ReadWord(hi2c, BQ27220_REG_VOLTAGE, &data->voltage_mv) != HAL_OK) return HAL_ERROR;

    // 2. Courant (mA - valeur signée en complément à 2)
    if (BQ27220_ReadWord(hi2c, BQ27220_REG_CURRENT, &raw_val) != HAL_OK) return HAL_ERROR;
    data->current_ma = (int16_t)raw_val;

    // 3. State of Charge (%)
    if (BQ27220_ReadWord(hi2c, BQ27220_REG_SOC, &data->soc_percent) != HAL_OK) return HAL_ERROR;

    // 4. Capacité restante (mAh)
    if (BQ27220_ReadWord(hi2c, BQ27220_REG_REMAINING_CAP, &data->remaining_cap_mah) != HAL_OK) return HAL_ERROR;

    // 5. Température (0.1 Kelvin -> Celsius)
    if (BQ27220_ReadWord(hi2c, BQ27220_REG_TEMPERATURE, &raw_val) != HAL_OK) return HAL_ERROR;
    data->temp_celsius = ((float)raw_val * 0.1f) - 273.15f;

    return HAL_OK;
}