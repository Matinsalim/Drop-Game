/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SER_Pin GPIO_PIN_13
#define SER_GPIO_Port GPIOC
#define LATCH_Pin GPIO_PIN_14
#define LATCH_GPIO_Port GPIOC
#define CLK_Pin GPIO_PIN_15
#define CLK_GPIO_Port GPIOC
#define MR_Pin GPIO_PIN_0
#define MR_GPIO_Port GPIOF
#define Ext_BTN_Pin GPIO_PIN_1
#define Ext_BTN_GPIO_Port GPIOF
#define ASK_IN_SIG_Pin GPIO_PIN_1
#define ASK_IN_SIG_GPIO_Port GPIOA
#define Ext_IO4_Pin GPIO_PIN_1
#define Ext_IO4_GPIO_Port GPIOB
#define Ext_IO3_Pin GPIO_PIN_2
#define Ext_IO3_GPIO_Port GPIOB
#define Ext_IO2_Pin GPIO_PIN_10
#define Ext_IO2_GPIO_Port GPIOB
#define Ext_IO1_Pin GPIO_PIN_11
#define Ext_IO1_GPIO_Port GPIOB
#define MCU_LED_Pin GPIO_PIN_12
#define MCU_LED_GPIO_Port GPIOB
#define Coin_Reader_Pin GPIO_PIN_13
#define Coin_Reader_GPIO_Port GPIOB
#define G_Pin GPIO_PIN_14
#define G_GPIO_Port GPIOB
#define F_Pin GPIO_PIN_15
#define F_GPIO_Port GPIOB
#define E_Pin GPIO_PIN_8
#define E_GPIO_Port GPIOA
#define D_Pin GPIO_PIN_9
#define D_GPIO_Port GPIOA
#define C_Pin GPIO_PIN_10
#define C_GPIO_Port GPIOA
#define B_Pin GPIO_PIN_11
#define B_GPIO_Port GPIOA
#define A_Pin GPIO_PIN_12
#define A_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
