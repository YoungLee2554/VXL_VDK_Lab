/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

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
#define LED_nRED_Pin GPIO_PIN_4
#define LED_nRED_GPIO_Port GPIOA
#define LED_nYELLOW_Pin GPIO_PIN_5
#define LED_nYELLOW_GPIO_Port GPIOA
#define LED_nGREEN_Pin GPIO_PIN_6
#define LED_nGREEN_GPIO_Port GPIOA
#define LED_eRED_Pin GPIO_PIN_7
#define LED_eRED_GPIO_Port GPIOA
#define PIN_0_Pin GPIO_PIN_0
#define PIN_0_GPIO_Port GPIOB
#define PIN_1_Pin GPIO_PIN_1
#define PIN_1_GPIO_Port GPIOB
#define PIN_2_Pin GPIO_PIN_2
#define PIN_2_GPIO_Port GPIOB
#define LED_eYELLOW_Pin GPIO_PIN_8
#define LED_eYELLOW_GPIO_Port GPIOA
#define LED_eGREEN_Pin GPIO_PIN_9
#define LED_eGREEN_GPIO_Port GPIOA
#define LED_sRED_Pin GPIO_PIN_10
#define LED_sRED_GPIO_Port GPIOA
#define LED_sYELLOW_Pin GPIO_PIN_11
#define LED_sYELLOW_GPIO_Port GPIOA
#define LED_sREDA12_Pin GPIO_PIN_12
#define LED_sREDA12_GPIO_Port GPIOA
#define LED_wRED_Pin GPIO_PIN_13
#define LED_wRED_GPIO_Port GPIOA
#define LED_wYELLOW_Pin GPIO_PIN_14
#define LED_wYELLOW_GPIO_Port GPIOA
#define LED_wGREEN_Pin GPIO_PIN_15
#define LED_wGREEN_GPIO_Port GPIOA
#define PIN_3_Pin GPIO_PIN_3
#define PIN_3_GPIO_Port GPIOB
#define PIN_4_Pin GPIO_PIN_4
#define PIN_4_GPIO_Port GPIOB
#define PIN_5_Pin GPIO_PIN_5
#define PIN_5_GPIO_Port GPIOB
#define PIN_6_Pin GPIO_PIN_6
#define PIN_6_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
