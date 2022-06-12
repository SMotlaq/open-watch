/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#define SAMPLING_TIME ((htim6.Init.Period+1)/((float)1e6)); /*** KALMAN_TIMER ***/
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MAX_EN_Pin GPIO_PIN_13
#define MAX_EN_GPIO_Port GPIOC
#define USB_DET_Pin GPIO_PIN_6
#define USB_DET_GPIO_Port GPIOF
#define USB_DET_EXTI_IRQn EXTI4_15_IRQn
#define KEY1_Pin GPIO_PIN_15
#define KEY1_GPIO_Port GPIOA
#define KEY1_EXTI_IRQn EXTI4_15_IRQn
#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOB
#define KEY2_EXTI_IRQn EXTI2_3_IRQn
#define KEY3_Pin GPIO_PIN_4
#define KEY3_GPIO_Port GPIOB
#define KEY3_EXTI_IRQn EXTI4_15_IRQn
#define KEY4_Pin GPIO_PIN_5
#define KEY4_GPIO_Port GPIOB
#define KEY4_EXTI_IRQn EXTI4_15_IRQn
#define MPU_INT_Pin GPIO_PIN_8
#define MPU_INT_GPIO_Port GPIOB
#define MPU_INT_EXTI_IRQn EXTI4_15_IRQn
#define MAX_INT_Pin GPIO_PIN_9
#define MAX_INT_GPIO_Port GPIOB
#define MAX_INT_EXTI_IRQn EXTI4_15_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
