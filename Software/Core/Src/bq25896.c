#include "bq25896.h"

#define BQ25896_I2C_TIMEOUT_MS 50U

static HAL_StatusTypeDef BQ25896_ReadReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *val) {
    return HAL_I2C_Mem_Read(hi2c, BQ25896_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, val, 1, BQ25896_I2C_TIMEOUT_MS);
}

static HAL_StatusTypeDef BQ25896_WriteReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t val) {
    return HAL_I2C_Mem_Write(hi2c, BQ25896_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, BQ25896_I2C_TIMEOUT_MS);
}

// Modifie uniquement certains bits d'un registre (Read-Modify-Write)
static HAL_StatusTypeDef BQ25896_UpdateBits(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t mask, uint8_t val) {
    uint8_t reg_val = 0;
    if (BQ25896_ReadReg(hi2c, reg, &reg_val) != HAL_OK) return HAL_ERROR;
    reg_val = (reg_val & ~mask) | (val & mask);
    return BQ25896_WriteReg(hi2c, reg, reg_val);
}

HAL_StatusTypeDef BQ25896_Init(I2C_HandleTypeDef *hi2c, BQ25896_Status_t *status, uint16_t charge_current_ma) {
    status->is_present = false;

    // 1. Vérifier présence sur le bus I2C
    if (HAL_I2C_IsDeviceReady(hi2c, BQ25896_I2C_ADDR, 3, BQ25896_I2C_TIMEOUT_MS) != HAL_OK) {
        return HAL_ERROR;
    }

    // 2. Vérifier le Part Number (bits [5:3] de REG14 doivent valoir 000 pour BQ25896)
    uint8_t part_id = 0;
    if (BQ25896_ReadReg(hi2c, BQ25896_REG14_PART_ID, &part_id) != HAL_OK) return HAL_ERROR;
    if ((part_id & 0x38U) != 0x00U) {
        return HAL_ERROR;
    }
    status->is_present = true;

    // 3. Désactiver le Watchdog I2C (REG07, bits [5:4] = 00)
    // Indispensable pour éviter que la puce ne réinitialise nos réglages toutes les 40s
    if (BQ25896_UpdateBits(hi2c, BQ25896_REG07_TIMER_WDT, 0x30U, 0x00U) != HAL_OK) return HAL_ERROR;

    // 4. Régler VSYS_MIN à 3.5V (bits [3:1] = 101) et activer la charge (bit 4 = 1) dans REG03
    // Protège l'alimentation système lors des pics de courant des moteurs
    if (BQ25896_UpdateBits(hi2c, BQ25896_REG03_SYS_MIN, 0x1EU, 0x1AU) != HAL_OK) return HAL_ERROR;

    // 5. Régler le courant de charge rapide (REG04, pas de 64 mA, max 3008 mA)
    // Pour une petite LiPo (ex: 500-1000 mAh), limiter à ~512 mA (0x08) ou 1024 mA (0x10)
    if (charge_current_ma > 3008U) charge_current_ma = 3008U;
    uint8_t ichg_code = (uint8_t)(charge_current_ma / 64U);
    if (BQ25896_UpdateBits(hi2c, BQ25896_REG04_ICHG, 0x7FU, ichg_code) != HAL_OK) return HAL_ERROR;

    // 6. Activer la conversion ADC en mode continu (1 Hz) pour lire VBAT et VBUS (REG02, bits [7:6] = 11)
    if (BQ25896_UpdateBits(hi2c, BQ25896_REG02_ADC_CTRL, 0xC0U, 0xC0U) != HAL_OK) return HAL_ERROR;

    return BQ25896_Update(hi2c, status);
}

HAL_StatusTypeDef BQ25896_SetChargeEnable(I2C_HandleTypeDef *hi2c, bool enable) {
    // Bit 4 (CHG_CONFIG) de REG03
    return BQ25896_UpdateBits(hi2c, BQ25896_REG03_SYS_MIN, 0x10U, enable ? 0x10U : 0x00U);
}

HAL_StatusTypeDef BQ25896_Update(I2C_HandleTypeDef *hi2c, BQ25896_Status_t *status) {
    uint8_t reg_val = 0;

    // 1. Lecture REG0B (État global)
    if (BQ25896_ReadReg(hi2c, BQ25896_REG0B_STATUS, &reg_val) != HAL_OK) return HAL_ERROR;
    status->charge_state   = (BQ25896_ChargeState_t)((reg_val >> 3) & 0x03U);
    status->vbus_connected = ((reg_val & 0x04U) != 0U); // Bit 2 = PG_STAT (Power Good)

    // 2. Lecture REG0C (Défauts - Note : REG0C est "Read to Clear", la lecture efface les anciens défauts)
    if (BQ25896_ReadReg(hi2c, BQ25896_REG0C_FAULT, &status->fault_reg) != HAL_OK) return HAL_ERROR;

    // 3. Lecture tension batterie (REG0E : offset 2304 mV + 20 mV par LSB)
    if (BQ25896_ReadReg(hi2c, BQ25896_REG0E_BATV, &reg_val) != HAL_OK) return HAL_ERROR;
    status->vbat_mv = 2304U + ((uint16_t)(reg_val & 0x7FU) * 20U);

    // 4. Lecture tension VBUS (REG11 : offset 2600 mV + 100 mV par LSB)
    if (BQ25896_ReadReg(hi2c, BQ25896_REG11_VBUSV, &reg_val) != HAL_OK) return HAL_ERROR;
    if (reg_val & 0x80U) { // Bit 7 = VBUS_GD (VBUS attaché)
        status->vbus_mv = 2600U + ((uint16_t)(reg_val & 0x7FU) * 100U);
    } else {
        status->vbus_mv = 0U;
    }

    return HAL_OK;
}