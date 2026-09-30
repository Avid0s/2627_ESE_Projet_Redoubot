/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Quartz_0_Pin GPIO_PIN_0
#define Quartz_0_GPIO_Port GPIOF
#define Quartz_1_Pin GPIO_PIN_1
#define Quartz_1_GPIO_Port GPIOF
#define PWM1_ML_Pin GPIO_PIN_0
#define PWM1_ML_GPIO_Port GPIOC
#define PWM2_ML_Pin GPIO_PIN_1
#define PWM2_ML_GPIO_Port GPIOC
#define PWM1_MR_Pin GPIO_PIN_2
#define PWM1_MR_GPIO_Port GPIOC
#define PWM2_MR_Pin GPIO_PIN_3
#define PWM2_MR_GPIO_Port GPIOC
#define PWM_SERVO_R_Pin GPIO_PIN_0
#define PWM_SERVO_R_GPIO_Port GPIOA
#define PWM_SERVO_L_Pin GPIO_PIN_1
#define PWM_SERVO_L_GPIO_Port GPIOA
#define RXD_HM10_Pin GPIO_PIN_2
#define RXD_HM10_GPIO_Port GPIOA
#define TXD_HM10_Pin GPIO_PIN_3
#define TXD_HM10_GPIO_Port GPIOA
#define PWM_NEOPIXEL_Pin GPIO_PIN_6
#define PWM_NEOPIXEL_GPIO_Port GPIOA
#define GPIO_BUSY_Pin GPIO_PIN_0
#define GPIO_BUSY_GPIO_Port GPIOB
#define RX_DF_Pin GPIO_PIN_10
#define RX_DF_GPIO_Port GPIOB
#define TX_DF_Pin GPIO_PIN_11
#define TX_DF_GPIO_Port GPIOB
#define BRK_HM10_Pin GPIO_PIN_12
#define BRK_HM10_GPIO_Port GPIOB
#define GPIO_NSLEEP_Pin GPIO_PIN_13
#define GPIO_NSLEEP_GPIO_Port GPIOB
#define GPIO_PWR_INT_Pin GPIO_PIN_14
#define GPIO_PWR_INT_GPIO_Port GPIOB
#define GPOUT_FG_Pin GPIO_PIN_15
#define GPOUT_FG_GPIO_Port GPIOB
#define SCL_MPU_Pin GPIO_PIN_8
#define SCL_MPU_GPIO_Port GPIOC
#define SDA_MPU_Pin GPIO_PIN_9
#define SDA_MPU_GPIO_Port GPIOC
#define INT_ADA_Pin GPIO_PIN_9
#define INT_ADA_GPIO_Port GPIOA
#define GPIO_OTG_EN_Pin GPIO_PIN_4
#define GPIO_OTG_EN_GPIO_Port GPIOB
#define XSHUT_TOF_Pin GPIO_PIN_5
#define XSHUT_TOF_GPIO_Port GPIOB
#define GPIO_TOF_Pin GPIO_PIN_6
#define GPIO_TOF_GPIO_Port GPIOB
#define STATE_HM10_Pin GPIO_PIN_9
#define STATE_HM10_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
