/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "ey_iran.h"
#include "MPU6050.h"
#include "Kalman.h"
#include "max30100_for_stm32_hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KALMAN_TIMER htim6
#define BUZZER_TIMER htim3
#define ADCVIB_TIMER htim1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int _index = 0;
char TxBuffer[24];
char RxBuffer[50];
uint32_t adcVals[1000];
float voltage = 0.0;
uint32_t sum = 0;
int debug = 0;

SAMPLE _sampleBuff[5];
uint8_t unreadSampleCount;
uint8_t _spo2;
uint8_t heartReat;
int16_t diff;

RTC_DateTypeDef CurrentDate;
RTC_TimeTypeDef CurrentTime;

RawData_Def myAccelRaw, myGyroRaw;
KalmanScaler KAccel;
Kalman2D1R KGyro;

MPU_ConfigTypeDef myMpuConfig;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void set_vib(int speed, int duration);
void set_tone(int freq, int duration);
void set_tone_vib(int freq, int duration);
float map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_ADC_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	
	/* I2C Scanner */
	/*
	uint8_t Buffer[25] = {0};
	uint8_t Space[] = " - ";
	uint8_t StartMSG[] = "Starting I2C Scanning: \n\r";
	uint8_t EndMSG[] = "\n\rDone! \n\r";
	int ret = 0;
	
	HAL_GPIO_WritePin(MAX_EN_GPIO_Port, MAX_EN_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart2, StartMSG, sizeof(StartMSG), 10000);
	for(int i=1; i<128; i++){
		ret = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 5);
		if (ret != HAL_OK)
			HAL_UART_Transmit(&huart2, Space, sizeof(Space), 10000);
		else if(ret == HAL_OK){
			sprintf((char*)Buffer, "0x%X", i);
			HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
		}
	}
	HAL_UART_Transmit(&huart2, EndMSG, sizeof(EndMSG), 10000);
	HAL_GPIO_WritePin(MAX_EN_GPIO_Port, MAX_EN_Pin, GPIO_PIN_RESET);	
	//*/

	
	//*
	//1. Initialise the MPU6050 module and I2C
		MPU6050_Init(&hi2c1);
	//2. Configure Accel and Gyro parameters
		myMpuConfig.Accel_Full_Scale = AFS_SEL_4g;
		myMpuConfig.ClockSource = Internal_8MHz;
		myMpuConfig.CONFIG_DLPF = DLPF_94A_98G_Hz;
		myMpuConfig.Gyro_Full_Scale = FS_SEL_2000;
		myMpuConfig.Sleep_Mode_Bit = 0;  //1: sleep mode, 0: normal mode
		MPU6050_Config(&myMpuConfig); 
	//*/
		
	RTC_DateTypeDef DateToBeSet;
	DateToBeSet.Year = 98;
	DateToBeSet.Month = 10;
	DateToBeSet.Date = 23;
	DateToBeSet.WeekDay = RTC_WEEKDAY_FRIDAY;
	
	RTC_TimeTypeDef TimeToBeSet;
	TimeToBeSet.Hours = 0;
	TimeToBeSet.Minutes = 0;
	TimeToBeSet.Seconds = 0;
	
	HAL_RTC_SetDate(&hrtc, &DateToBeSet, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &TimeToBeSet, RTC_FORMAT_BIN);
		
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)RxBuffer, 5); 
	
	HAL_ADC_Start_DMA(&hadc, adcVals, 100);
	HAL_TIM_Base_Start(&ADCVIB_TIMER);
	
	kalman_scaler_init(&KAccel, 0.3, 900);
	kalman_2d1r_init(&KGyro, 0.1, 1);
	HAL_TIM_Base_Start_IT(&KALMAN_TIMER);	
	
	//HAL_GPIO_WritePin(MAX_EN_GPIO_Port, MAX_EN_Pin, GPIO_PIN_SET);
  //max30102_init();
	 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
	{
		/* Music + Vibration*/
		/*
		for(int thisNote=0; thisNote<sizeof(melody)/sizeof(int); thisNote++){
			set_tone_vib(melody[thisNote], 1000/noteDurations[thisNote]);
		}
		//*/
		 
		/* Battery level + Date and Time */
		/*
		HAL_RTC_GetDate(&hrtc, &CurrentDate, RTC_FORMAT_BIN);
		HAL_RTC_GetTime(&hrtc, &CurrentTime, RTC_FORMAT_BIN);
		uint8_t psize = sprintf(TxBuffer, "Battery: %.1f%% - DateTime: 19%02d-%02d-%02d %02d:%02d:%02d\n", map(voltage, 3000, 3907, 0, 100),
														CurrentDate.Year , CurrentDate.Month  , CurrentDate.Date,
														CurrentTime.Hours, CurrentTime.Minutes, CurrentTime.Seconds);
		HAL_UART_Transmit(&huart2, (uint8_t*)TxBuffer, psize, 1000);
		
		//psize = sprintf(TxBuffer, "|____ HeartRate: %3d - SpO2: %3d \n", heartReat, _spo2);
		//HAL_UART_Transmit(&huart2, (uint8_t*)TxBuffer, psize, 1000);
		
		HAL_Delay(1000);
		//*/
		
		/* Vibraion */
		/*
		set_vib(0, 800);
		set_vib(300, 300);
		set_vib(900, 100);
		//*/
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  } 
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&KALMAN_TIMER){
		/* Gyro */
		//*
		MPU6050_Get_Accel_RawData(&myAccelRaw);
		MPU6050_Get_Gyro_RawData(&myGyroRaw);
//		uint8_t psize = sprintf(TxBuffer, "Accel x: %.1f y: %.1f z: %.1f - Gyro: x: %.1f y: %.1f z: %.1f\n\r",
//														myAccelScaled.x, myAccelScaled.y, myAccelScaled.z,
//														myGyroScaled.x , myGyroScaled.y , myGyroScaled.z);
		TxBuffer[0] = (uint8_t)(myAccelRaw.x & 0x00FF);
		TxBuffer[1] = (uint8_t)((myAccelRaw.x & 0xFF00)>>8);
		
		TxBuffer[2] = (uint8_t)(myAccelRaw.y & 0x00FF);
		TxBuffer[3] = (uint8_t)((myAccelRaw.y & 0xFF00)>>8);

		TxBuffer[4] = (uint8_t)(myAccelRaw.z & 0x00FF);
		TxBuffer[5] = (uint8_t)((myAccelRaw.z & 0xFF00)>>8);
		
		TxBuffer[6] = (uint8_t)(myGyroRaw.x & 0x00FF);
		TxBuffer[7] = (uint8_t)((myGyroRaw.x & 0xFF00)>>8);
		
		TxBuffer[8] = (uint8_t)(myGyroRaw.y & 0x00FF);
		TxBuffer[9] = (uint8_t)((myGyroRaw.y & 0xFF00)>>8);
		
		TxBuffer[10] = (uint8_t)(myGyroRaw.z & 0x00FF);
		TxBuffer[11] = (uint8_t)((myGyroRaw.z & 0xFF00)>>8);
		
		// Filtering ---------------------------------------
		
		/* Accelerometer */
		KAccel.z_x = myAccelRaw.x;
		KAccel.z_y = myAccelRaw.y;
		KAccel.z_z = myAccelRaw.z;
		kalman_scaler_update(&KAccel);
		
		/* Gyroscope (X and Z) */
		KGyro.z_x = myGyroRaw.x;
		KGyro.z_y = myGyroRaw.y;
		KGyro.z_z = myGyroRaw.z;
		kalman_2d1r_update(&KGyro);

		// Filtererd data ----------------------------------
		
		TxBuffer[12] = (uint8_t)(KAccel.x_hat & 0x00FF);
		TxBuffer[13] = (uint8_t)((KAccel.x_hat & 0xFF00)>>8);
		
		TxBuffer[14] = (uint8_t)(KAccel.y_hat & 0x00FF);
		TxBuffer[15] = (uint8_t)((KAccel.y_hat & 0xFF00)>>8);

		TxBuffer[16] = (uint8_t)(KAccel.z_hat & 0x00FF);
		TxBuffer[17] = (uint8_t)((KAccel.z_hat & 0xFF00)>>8);
		
		TxBuffer[18] = (uint8_t)(KGyro.wx_hat & 0x00FF);
		TxBuffer[19] = (uint8_t)((KGyro.wx_hat & 0xFF00)>>8);
		
		TxBuffer[20] = (uint8_t)(KGyro.wy_hat & 0x00FF);
		TxBuffer[21] = (uint8_t)((KGyro.wy_hat & 0xFF00)>>8);
		
		TxBuffer[22] = (uint8_t)(KGyro.wz_hat & 0x00FF);
		TxBuffer[23] = (uint8_t)((KGyro.wz_hat & 0xFF00)>>8);
		
		HAL_UART_Transmit(&huart1, (uint8_t*)TxBuffer, 24, 10); 
		//*/
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == MAX_INT_Pin) {
		//max30102_cal();
    //_spo2 = max30102_getSpO2();
    //heartReat = max30102_getHeartRate();
		//diff = max30102_getDiff();
		unreadSampleCount = max30102_getUnreadSampleCount();
    max30102_getFIFO(_sampleBuff, unreadSampleCount);
		uint8_t psize = sprintf(TxBuffer, "%d\n", _sampleBuff[0].iRed);
		HAL_UART_Transmit(&huart2, (uint8_t*)TxBuffer, psize, 1000);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	//HAL_UART_Transmit(&huart2, (uint8_t*)RxBuffer, 5, 1000);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
  _index++;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	sum = 0;
	for(int i = 0; i<1000; i++){
		sum += adcVals[i];
	}
	voltage = sum/100.0;
	
}

void set_vib(int speed, int duration){
	HAL_TIM_PWM_Start(&ADCVIB_TIMER, TIM_CHANNEL_4);
	__HAL_TIM_SET_AUTORELOAD(&ADCVIB_TIMER, 5000);
	__HAL_TIM_SET_COMPARE(&ADCVIB_TIMER, TIM_CHANNEL_4, speed);
	HAL_Delay(duration);
	HAL_TIM_PWM_Stop(&ADCVIB_TIMER, TIM_CHANNEL_4);
	HAL_Delay((int)(duration*0.3));
}

void set_tone(int freq, int duration){
	HAL_TIM_PWM_Start(&BUZZER_TIMER, TIM_CHANNEL_3);
	__HAL_TIM_SET_AUTORELOAD(&BUZZER_TIMER, 1000000/freq);
	__HAL_TIM_SET_COMPARE(&BUZZER_TIMER, TIM_CHANNEL_3, 1000000/freq/2);
	HAL_Delay(duration);
	HAL_TIM_PWM_Stop(&BUZZER_TIMER, TIM_CHANNEL_3);
	HAL_Delay((int)(duration*0.3));
}

void set_tone_vib(int freq, int duration){
	HAL_TIM_PWM_Start(&ADCVIB_TIMER, TIM_CHANNEL_4);
	__HAL_TIM_SET_AUTORELOAD(&ADCVIB_TIMER, 5000);
	__HAL_TIM_SET_COMPARE(&ADCVIB_TIMER, TIM_CHANNEL_4, (int)map(freq, 260, 500, 2000, 5000));
	HAL_TIM_PWM_Start(&BUZZER_TIMER, TIM_CHANNEL_3);
	__HAL_TIM_SET_AUTORELOAD(&BUZZER_TIMER, 1000000/freq);
	__HAL_TIM_SET_COMPARE(&BUZZER_TIMER, TIM_CHANNEL_3, 1000000/freq/2);
	HAL_Delay(duration);
	HAL_TIM_PWM_Stop(&BUZZER_TIMER, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&ADCVIB_TIMER, TIM_CHANNEL_4);
	HAL_Delay((int)(duration*0.3));
}

float map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max){
  return (x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

