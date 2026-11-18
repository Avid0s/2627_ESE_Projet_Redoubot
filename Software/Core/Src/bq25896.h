#ifndef BQ25896_H
#define BQ25896_H

#include "stm32g4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

// Adresse I2C 7-bit fixe du BQ25896 (0x6B) décalée pour la HAL STM32
#define BQ25896_I2C_ADDR            (0x6BU << 1)

// Registres principaux du BQ25896 (Datasheet SLUSCC4)
#define BQ25896_REG00_ILIM          0x00U // Input Current Limit & HIZ
#define BQ25896_REG02_ADC_CTRL      0x02U // Contrôle de l'ADC interne
#define BQ25896_REG03_SYS_MIN       0x03U // Charge enable & VSYS_MIN
#define BQ25896_REG04_ICHG          0x04U // Fast Charge Current Limit
#define BQ25896_REG07_TIMER_WDT     0x07U // Watchdog Timer & Safety Timer
#define BQ25896_REG0B_STATUS        0x0BU // État VBUS, Charge Status, Power Good
#define BQ25896_REG0C_FAULT         0x0CU // Registre des défauts (NTC/TS, OVP, etc.)
#define BQ25896_REG0E_BATV          0x0EU // Tension batterie mesurée par ADC
#define BQ25896_REG11_VBUSV         0x11U // Tension USB/Chargeur mesurée par ADC
#define BQ25896_REG14_PART_ID       0x14U // Révision & Reset (Part Number = 000)

// États de charge (bits CHRG_STAT[4:3] de REG0B)
typedef enum {
    BQ25896_CHRG_NOT_CHARGING = 0,
    BQ25896_CHRG_PRE_CHARGE   = 1,
    BQ25896_CHRG_FAST_CHARGE  = 2,
    BQ25896_CHRG_DONE         = 3
} BQ25896_ChargeState_t;

// Structure d'état du chargeur
typedef struct {
    bool                  is_present;    // True si le composant répond sur l'I2C
    bool                  vbus_connected;// True si câble USB/chargeur branché (PG_STAT)
    BQ25896_ChargeState_t charge_state;  // État actuel de la charge
    uint8_t               fault_reg;     // Valeur brute REG0C (0 = aucun défaut)
    uint16_t              vbat_mv;       // Tension VBAT lue par l'ADC du BQ25896
    uint16_t              vbus_mv;       // Tension VBUS lue par l'ADC du BQ25896
} BQ25896_Status_t;

// Prototypes
HAL_StatusTypeDef BQ25896_Init(I2C_HandleTypeDef *hi2c, BQ25896_Status_t *status, uint16_t charge_current_ma);
HAL_StatusTypeDef BQ25896_Update(I2C_HandleTypeDef *hi2c, BQ25896_Status_t *status);
HAL_StatusTypeDef BQ25896_SetChargeEnable(I2C_HandleTypeDef *hi2c, bool enable);

#endif // BQ25896_H