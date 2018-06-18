/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define Interlock_In_Status_Pin GPIO_PIN_0
#define Interlock_In_Status_GPIO_Port GPIOC
#define FLT_Status_Pin GPIO_PIN_1
#define FLT_Status_GPIO_Port GPIOC
#define FLT_NR_Status_Pin GPIO_PIN_2
#define FLT_NR_Status_GPIO_Port GPIOC
#define IMD_Status_Pin GPIO_PIN_3
#define IMD_Status_GPIO_Port GPIOC
#define AMS_Status_Pin GPIO_PIN_1
#define AMS_Status_GPIO_Port GPIOA
#define BSPD_Status_Pin GPIO_PIN_2
#define BSPD_Status_GPIO_Port GPIOA
#define BAT_SENSE_Pin GPIO_PIN_4
#define BAT_SENSE_GPIO_Port GPIOA
#define Interlock_Reset_Pin GPIO_PIN_7
#define Interlock_Reset_GPIO_Port GPIOE
#define IMD_Observe_Pin GPIO_PIN_8
#define IMD_Observe_GPIO_Port GPIOE
#define AMS_Observe_Pin GPIO_PIN_9
#define AMS_Observe_GPIO_Port GPIOE
#define BSPD_Observe_Pin GPIO_PIN_10
#define BSPD_Observe_GPIO_Port GPIOE
#define FLT_NR_Observe_Pin GPIO_PIN_11
#define FLT_NR_Observe_GPIO_Port GPIOE
#define FLT_Observe_Pin GPIO_PIN_12
#define FLT_Observe_GPIO_Port GPIOE
#define Interlock_In_Observe_Pin GPIO_PIN_13
#define Interlock_In_Observe_GPIO_Port GPIOE
#define FLT_Pin GPIO_PIN_12
#define FLT_GPIO_Port GPIOB
#define FLT_NR_Pin GPIO_PIN_13
#define FLT_NR_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
