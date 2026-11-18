#ifndef FUEL_GAUGE_H
#define FUEL_GAUGE_H

#include "stm32g4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

// Adresse I2C 7-bit (0x55) décalée pour la HAL STM32
#define BQ27220_I2C_ADDR            (0x55U << 1)

// Registres des commandes standards (Standard Commands - SLUUBD4)
#define BQ27220_REG_CONTROL         0x00U
#define BQ27220_REG_TEMPERATURE     0x06U // Unité : 0.1 K
#define BQ27220_REG_VOLTAGE         0x08U // Unité : mV
#define BQ27220_REG_BATTERY_STATUS  0x0AU // Flags d'état
#define BQ27220_REG_CURRENT         0x0CU // Unité : mA (signé)
#define BQ27220_REG_REMAINING_CAP   0x10U // Unité : mAh
#define BQ27220_REG_FULL_CHARGE_CAP 0x12U // Unité : mAh
#define BQ27220_REG_SOC             0x2CU // State of Charge (%)

// Sous-commandes Control() utiles
#define BQ27220_SUBCMD_DEVICE_TYPE  0x0001U // Doit retourner 0x0220

// Structure regroupant l'état de la batterie
typedef struct {
    uint16_t voltage_mv;        // Tension batterie en mV
    int16_t  current_ma;        // Courant instantané en mA (<0 = décharge, >0 = charge)
    uint16_t soc_percent;       // Pourcentage de charge (0 à 100%)
    uint16_t remaining_cap_mah; // Capacité restante en mAh
    float    temp_celsius;      // Température interne en °C
    bool     is_present;        // True si le composant répond sur le bus I2C
} BQ27220_Data_t;

// Prototypes des fonctions
HAL_StatusTypeDef BQ27220_Init(I2C_HandleTypeDef *hi2c, BQ27220_Data_t *data);
HAL_StatusTypeDef BQ27220_Update(I2C_HandleTypeDef *hi2c, BQ27220_Data_t *data);
HAL_StatusTypeDef BQ27220_GetDeviceID(I2C_HandleTypeDef *hi2c, uint16_t *device_id);

#endif // FUEL_GAUGE_H