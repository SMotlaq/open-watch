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
#include "ey_iran.h"
#include "MPU6050.h"
#include "Kalman.h"
#include "max30102_for_stm32_hal.h"
#include "ssd1306.h"
#include "fonts.h"
#include "stdio.h"
#include "pages.h"
#include "xfer.h"
#include "system.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MDELAY_TIMER_VALUE (TIM17->CNT)
#define MDELAY_TIMER		htim17		// No IT - each 0.001 seconds - used to create a timer-based delay function (MHAL_Delay)
#define SCREEN_TIMER_RESET (TIM16->CNT = 0)
#define SCREEN_TIMER		htim16		// IT mode - each 4 seconds
#define BATERY_TIMER		htim15		// IT mode - each 0.01 seconds
#define SYSTEM_TIMER		htim14		// IT mode - each 0.25 seconds
#define KALMAN_TIMER		htim6			// IT mode - each 0.001 seconds
#define BUZZER_TIMER		htim3			// PWM mode - 1KHz - Value from 0 to 999
#define VIBMOT_TIMER		htim1			// PWM mode - 1KHz - Value from 0 to 4999

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
System sys;

int display_fake_delay = 0;
int timeout_counter = 0;
int _index = 0; 

/* Batery Level */
uint32_t adcVals[128];
uint32_t sum = 0;

uint8_t TxBuffer[12];

/* PPG Sensor */
max30102_t max30102;
uint8_t en_reg[2] = {0};

/* Movement */
MPU_ConfigTypeDef myMpuConfig;
RawData_Def myAccelRaw, myGyroRaw;
KalmanScaler KAccel;
Kalman2D1R KGyro;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void max30102_plot(uint32_t ir_sample, uint32_t red_sample);
void max30102_turn_on(void);
void max30102_turn_off(void);
void set_vib(int speed, int duration);
void set_tone(int freq, int duration);
void set_tone_vib(int freq, int duration);
void beeb_beeb(System* sys, int freq, int duration);
void MHAL_Delay(uint32_t Delay);

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
  MX_TIM15_Init();
  MX_TIM14_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
	
	/* // I2C scanner
		int ret = 0;		
		HAL_GPIO_WritePin(MAX_EN_GPIO_Port, MAX_EN_Pin, GPIO_PIN_SET);
		DEBUG("Starting I2C Scanning: \n\r ");
		for(int i=1; i<128; i++){
			ret = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 5);
			if (ret != HAL_OK)
				DEBUG(" - ");
			else if(ret == HAL_OK)
				DEBUG("0x%X", i);
		}
		DEBUG("\n\rDone! \n\r ");
		HAL_GPIO_WritePin(MAX_EN_GPIO_Port, MAX_EN_Pin, GPIO_PIN_RESET);
	//*/
	
	/*/ // Date and Time setting
	DEBUG("Date and Time setting...  ");
		RTC_DateTypeDef DateToBeSet;
		DateToBeSet.Year = 22;
		DateToBeSet.Month = 8;
		DateToBeSet.Date = 4;
		DateToBeSet.WeekDay = RTC_WEEKDAY_THURSDAY;
		
		RTC_TimeTypeDef TimeToBeSet;
		TimeToBeSet.Hours = 17;
		TimeToBeSet.Minutes = 16;
		TimeToBeSet.Seconds = 30;
		
		HAL_RTC_SetDate(&hrtc, &DateToBeSet, RTC_FORMAT_BIN);
		HAL_RTC_SetTime(&hrtc, &TimeToBeSet, RTC_FORMAT_BIN);
	DEBUG("  Done! \n\r ");
	HAL_Delay(2000);
	//*/
	
	/*/ // Alarm setting
	DEBUG("Alarm setting...  ");
		RTC_AlarmTypeDef sAlarm = {0};
		sAlarm.AlarmTime.Hours = 17;
		sAlarm.AlarmTime.Minutes = 0;
		sAlarm.AlarmTime.Seconds = 0;
		sAlarm.AlarmTime.SubSeconds = 0;
		sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
		sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
		sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
		sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
		sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_FRIDAY;
		sAlarm.Alarm = RTC_ALARM_A;
		HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
	DEBUG("  Done! \n\r ");
	HAL_Delay(2000);
	//*/
	
	//* // Display initiation
	DEBUG("Display initiation...  ");
		ssd1306_Init(&hi2c1);
	DEBUG("  Done! \n\r ");
	HAL_Delay(2000);
	//*/
	
	//* // System initiation
	DEBUG("System initiation...  ");
		sys = sys_init();
		HAL_TIM_Base_Start_IT(&SYSTEM_TIMER);
	DEBUG("  Done! \n\r ");
	HAL_Delay(2000);
	//*/
		
	//* // MPU6050 initiation
	DEBUG("MPU6050 initiation...  ");
		MPU6050_Init(&hi2c1);
		myMpuConfig.Accel_Full_Scale = AFS_SEL_4g;
		myMpuConfig.ClockSource = Internal_8MHz;
		myMpuConfig.CONFIG_DLPF = DLPF_94A_98G_Hz;
		myMpuConfig.Gyro_Full_Scale = FS_SEL_2000;
		myMpuConfig.Sleep_Mode_Bit = 0;
		MPU6050_Config(&myMpuConfig); 
	DEBUG("  Done! \n\r ");
	HAL_Delay(2000);
	//*/
		 
	//* // Bluetooth initiation
	DEBUG("Bluetooth initiation...  ");
		HAL_UART_Receive_DMA(&BLUETOOTH_UART, sys.RxBuffer, RX_BUFFER_SIZE);
	
	/* // Bluetooth settings
		uint8_t psize = 0;
		char setting_buffer[25];
		psize = sprintf(setting_buffer, "AT+NAMEOpenWatch");
		HAL_UART_Transmit(&BLUETOOTH_UART, (uint8_t*)setting_buffer, psize, 1000);
		psize = sprintf(setting_buffer, "AT+PIN8569");
		HAL_UART_Transmit(&BLUETOOTH_UART, (uint8_t*)setting_buffer, psize, 1000);
		psize = sprintf(setting_buffer, "AT+BAUD8"); // 115200 bps
		HAL_UART_Transmit(&BLUETOOTH_UART, (uint8_t*)setting_buffer, psize, 1000);
		psize = sprintf(setting_buffer, "AT");
		HAL_UART_Transmit(&BLUETOOTH_UART, (uint8_t*)setting_buffer, psize, 1000);
		while(1);
	//*/
	
	DEBUG("  Done! \n\r ");
	HAL_Delay(2000);
	//*/
	
	//* // Baettery level ADC settings
	DEBUG("Baettery level ADC settings...  ");
		HAL_ADC_Start_DMA(&hadc, adcVals, 128);
		HAL_TIM_Base_Start(&BATERY_TIMER); 
	DEBUG("  Done! \n\r ");
	HAL_Delay(2000);
	//*/
	
	//* // Kalman filter initiation
	DEBUG("Kalman filter initiation...  ");
		kalman_scaler_init(&KAccel, 0.8, 900);
		kalman_2d1r_init(&KGyro, 0.1, 1);
		HAL_TIM_Base_Start_IT(&KALMAN_TIMER);	
	DEBUG("  Done! \n\r ");
	HAL_Delay(2000);
	//*/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
	{
		
		// Ringing in case of activated alarms
		if(getRinging(&sys)){
			for(int thisNote=0; thisNote<sizeof(melody)/sizeof(int); thisNote++){
				beeb_beeb(&sys, melody[thisNote], 1200/noteDurations[thisNote]);
				if(!getRinging(&sys))
					break;
			}
		} 

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &KALMAN_TIMER){
		MPU6050_Get_RawData(&myAccelRaw, &myGyroRaw);
		
		/*
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
		//*/
		
		// Filtering ---------------------------------------
		
		/* Accelerometer */
		KAccel.z_x = myAccelRaw.x;
		#if ALLXYZ_MODE
			KAccel.z_y = myAccelRaw.y;
			KAccel.z_z = myAccelRaw.z;
		#endif
		kalman_scaler_update(&KAccel); 
		
		/* Gyroscope */
		#if ALLXYZ_MODE
			KGyro.z_x = myGyroRaw.x;
			KGyro.z_z = myGyroRaw.z;
		#endif
		KGyro.z_y = myGyroRaw.y;
		kalman_2d1r_update(&KGyro); 

		/* Pedometer */
		if((KAccel.x_hat >= 3200)&&(!getPedFlag(&sys))){
			setPedFlag(&sys,1);
			sys.pedometer += 2;
			//DEBUG("%d\n\r",sys.pedometer);
		}
		else if((KAccel.x_hat < 2500)&&(getPedFlag(&sys))){ 
			setPedFlag(&sys,0);
		}
		
		/* Enable screen */
		if((KGyro.wy_hat >= 4000)&&(!getScreenFlag(&sys))){
			setScreenFlag(&sys,1);
		}
		else if((KAccel.x_hat < 400)&&(getScreenFlag(&sys))){
			setScreenFlag(&sys,0);
			HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
			setScreenEable(&sys,1);
		}
		
		// Filtererd data ----------------------------------
		/*
		TxBuffer[12-12] = (uint8_t)(KAccel.x_hat & 0x00FF);
		TxBuffer[13-12] = (uint8_t)((KAccel.x_hat & 0xFF00)>>8);
		
		TxBuffer[14-12] = (uint8_t)(KAccel.y_hat & 0x00FF);
		TxBuffer[15-12] = (uint8_t)((KAccel.y_hat & 0xFF00)>>8);

		TxBuffer[16-12] = (uint8_t)(KAccel.z_hat & 0x00FF);
		TxBuffer[17-12] = (uint8_t)((KAccel.z_hat & 0xFF00)>>8);
		
		TxBuffer[18-12] = (uint8_t)(KGyro.wx_hat & 0x00FF);
		TxBuffer[19-12] = (uint8_t)((KGyro.wx_hat & 0xFF00)>>8);
		
		TxBuffer[20-12] = (uint8_t)(KGyro.wy_hat & 0x00FF);
		TxBuffer[21-12] = (uint8_t)((KGyro.wy_hat & 0xFF00)>>8);
		
		TxBuffer[22-12] = (uint8_t)(KGyro.wz_hat & 0x00FF);
		TxBuffer[23-12] = (uint8_t)((KGyro.wz_hat & 0xFF00)>>8);
		//*/
		
		//HAL_UART_Transmit(&BLUETOOTH_UART, (uint8_t*)TxBuffer, 12, 10);
	}
	else if(htim == &SYSTEM_TIMER){
		if(getScreenEable(&sys)){
			sys.battery = map(sys.voltage, 3000, 3907, 0, 100);
			switch (sys.state)
			{
				case ack_waiting:
						HAL_TIM_Base_Stop(&SCREEN_TIMER);
						render_ack_waiting(&sys);
					break;
				case hello:
						if(display_fake_delay<16){
							render_connected(&sys);
							display_fake_delay++;
						}
						else{
							sys.state = home;
							display_fake_delay = 0;
							SCREEN_TIMER_RESET;
							HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
						}
					break;
				case home:
						HAL_RTC_GetTime(&hrtc, &sys.Time, RTC_FORMAT_BIN);
						HAL_RTC_GetDate(&hrtc, &sys.Date, RTC_FORMAT_BIN);
						render_home(&sys);
					break;
				case bloody_hell:
						HAL_TIM_Base_Stop(&SCREEN_TIMER);
						render_bloody_hell(&sys);
					break;
				case pedomedo:
						HAL_TIM_Base_Stop(&SCREEN_TIMER);
						render_pedomedo(&sys);
					break;
				case about:
						HAL_TIM_Base_Stop(&SCREEN_TIMER);
						render_about(&sys);
					break;
				case ringing:
						HAL_TIM_Base_Stop(&SCREEN_TIMER);
						render_ringing(&sys);
					break;
			}
			if(sys.state != bloody_hell){
				if(timeout_counter < 4){
					timeout_counter++;
				}
				else{
					setConnected(&sys, getSHRreached(&sys));
					sys.ack = getConnected(&sys);
					setSHRreached(&sys, 0);
					timeout_counter = 0;
				}
			}
		}
		else{
			ssd1306_Fill(Black);
			ssd1306_UpdateScreen(&hi2c1);
		}
	}
	else if(htim == &SCREEN_TIMER){
		HAL_TIM_Base_Stop(&SCREEN_TIMER);
		setScreenEable(&sys, 0);
		SCREEN_TIMER_RESET;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == MAX_INT_Pin) {
		max30102_interrupt_handler(&max30102);
	}
	else if(GPIO_Pin == KEY4_Pin){
		DEBUG("UP PRESSED \n\r");
		if(!getScreenEable(&sys)){
			//
		}
		else{
			SCREEN_TIMER_RESET;
			switch (sys.state)
			{
				case ack_waiting:
						// accept
						sys.ack = 1;
						TxGenerator(&sys);
					break;
				case hello:
						//
					break;
				case home:
						sys.state = about;
					break;
				case bloody_hell:
						max30102_turn_off();
						HAL_TIM_Base_Start_IT(&BATERY_TIMER);
						HAL_TIM_Base_Start_IT(&KALMAN_TIMER);
				
						sys.state = home;
						SCREEN_TIMER_RESET;
						HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
					break;
				case pedomedo:
						HAL_TIM_Base_Stop(&KALMAN_TIMER);
						HAL_TIM_Base_Stop(&BATERY_TIMER);
						max30102_turn_on();
						sys.state = bloody_hell;
					break;
				case about:
						sys.state = pedomedo;
					break;
				case ringing:
						SCREEN_TIMER_RESET;
						setRinging(&sys, 0);
						HAL_TIM_Base_Start_IT(&KALMAN_TIMER);
						HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
						sys.state = home;
					break;
			}
			beeb_beeb(&sys, 500, 100);
		}
	}
	else if(GPIO_Pin == KEY3_Pin){
		DEBUG("RIGHT PRESSED \n\r");
		if(!getScreenEable(&sys)){
			//
		}
		else{
			SCREEN_TIMER_RESET;
			switch (sys.state)
			{
				case ack_waiting:
						sys.state = ringing;
					break;
				case hello:
						//
					break;
				case home:
						sys.state = ack_waiting;
					break;
				case bloody_hell:
						//
					break;
				case pedomedo:
						//
					break;
				case about:
						//
					break;
				case ringing:
						SCREEN_TIMER_RESET;
						setRinging(&sys, 0);
						HAL_TIM_Base_Start_IT(&KALMAN_TIMER);
						HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
						sys.state = home; 
					break;
			}
			beeb_beeb(&sys, 500, 100);
		}
	}
	else if(GPIO_Pin == KEY2_Pin){
		DEBUG("DOWN PRESSED \n\r");
		if(!getScreenEable(&sys)){
			sys.state = home;
			HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
			setScreenEable(&sys,1);
		}
		else{
			SCREEN_TIMER_RESET;
			switch (sys.state)
			{
				case ack_waiting:
						// reject
						sys.ack = 0;
						TxGenerator(&sys);
						sys.state = home;
						setScreenEable(&sys,0);
					break;
				case hello:
						//
					break;
				case home:
						HAL_TIM_Base_Stop(&KALMAN_TIMER);
						HAL_TIM_Base_Stop(&BATERY_TIMER);
						max30102_turn_on();
						sys.state = bloody_hell;
					break;
				case bloody_hell:
						max30102_turn_off();
						HAL_TIM_Base_Start_IT(&BATERY_TIMER);
						HAL_TIM_Base_Start_IT(&KALMAN_TIMER);
						sys.state = pedomedo;
					break;
				case pedomedo:
						sys.state = about;
					break;
				case about:
						SCREEN_TIMER_RESET;
						HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
						sys.state = home;
					break;
				case ringing:
						SCREEN_TIMER_RESET;
						setRinging(&sys, 0);
						HAL_TIM_Base_Start_IT(&KALMAN_TIMER);
						HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
						sys.state = home;
					break;
			}
			beeb_beeb(&sys, 500, 100);
		}
	}
	else if(GPIO_Pin == KEY1_Pin){
		DEBUG("LEFT PRESSED \n\r");
		if(!getScreenEable(&sys)){
			//
		}
		else{
			SCREEN_TIMER_RESET;
			switch (sys.state)
			{
				case ack_waiting:
						SCREEN_TIMER_RESET;
						setRinging(&sys, 0);
						HAL_TIM_Base_Start_IT(&KALMAN_TIMER);
						HAL_TIM_Base_Start_IT(&SCREEN_TIMER);
						sys.state = home;
					break;
				case hello:
						//
					break;
				case home:
						sys.state = ringing;
					break;
				case bloody_hell:
						//
					break;
				case pedomedo:
						//
					break;
				case about:
						//
					break;
				case ringing:
						sys.state = ack_waiting;
					break;
			}
			beeb_beeb(&sys, 500, 100);
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart==&BLUETOOTH_UART){
		RxParser(&sys);
	}
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
	if(sys.isAlarmEnabled){
		sys.state = ringing;
		HAL_TIM_Base_Stop(&KALMAN_TIMER);
		setRinging(&sys, 1);
		setScreenEable(&sys,1);
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	sum = 0;
	for(uint32_t i = 0; i<128; i++)
		sum += adcVals[i];
	sys.voltage = sum >> 7;
	
}
void max30102_plot(uint32_t ir_sample, uint32_t red_sample){
	append_PPG_buffer(&sys, ir_sample, red_sample);
}
void max30102_turn_on(void){
	clear_PPG_buffer(&sys);
	
	HAL_GPIO_WritePin(MAX_EN_GPIO_Port, MAX_EN_Pin, GPIO_PIN_SET);
	
	// Initiation
	max30102_init(&max30102, &hi2c1);
	max30102_reset(&max30102);
	max30102_clear_fifo(&max30102);
	max30102_set_fifo_config(&max30102, max30102_smp_ave_8, 1, 7);
	
	// Sensor settings
	max30102_set_led_pulse_width(&max30102, max30102_pw_16_bit);
	max30102_set_adc_resolution(&max30102, max30102_adc_2048);
	max30102_set_sampling_rate(&max30102, max30102_sr_1000);
	max30102_set_led_current_1(&max30102, 6.2);
	max30102_set_led_current_2(&max30102, 6.2);

	// Enter SpO2 mode
	max30102_set_mode(&max30102, max30102_spo2);
	max30102_set_a_full(&max30102, 1);
	
	// Initiate 1 temperature measurement
	max30102_set_die_temp_en(&max30102, 1);
	max30102_set_die_temp_rdy(&max30102, 1);

	max30102_read(&max30102, 0x00, en_reg, 1);
}
void max30102_turn_off(void){
	HAL_GPIO_WritePin(MAX_EN_GPIO_Port, MAX_EN_Pin, GPIO_PIN_RESET);
}
void set_vib(int freq, int duration){
	HAL_TIM_PWM_Start(&VIBMOT_TIMER, TIM_CHANNEL_4);
	__HAL_TIM_SET_AUTORELOAD(&VIBMOT_TIMER, 5000);
	__HAL_TIM_SET_COMPARE(&VIBMOT_TIMER, TIM_CHANNEL_4, (int)map(freq, 260, 500, 2000, 5000));
	MHAL_Delay(duration);
	HAL_TIM_PWM_Stop(&VIBMOT_TIMER, TIM_CHANNEL_4);
	MHAL_Delay((int)(duration*0.3));
}

void set_tone(int freq, int duration){
	HAL_TIM_PWM_Start(&BUZZER_TIMER, TIM_CHANNEL_3);
	__HAL_TIM_SET_AUTORELOAD(&BUZZER_TIMER, 1000000/freq);
	__HAL_TIM_SET_COMPARE(&BUZZER_TIMER, TIM_CHANNEL_3, 1000000/freq/2);
	MHAL_Delay(duration);
	HAL_TIM_PWM_Stop(&BUZZER_TIMER, TIM_CHANNEL_3);
	MHAL_Delay((int)(duration*0.3));
}

void set_tone_vib(int freq, int duration){
	HAL_TIM_PWM_Start(&VIBMOT_TIMER, TIM_CHANNEL_4);
	__HAL_TIM_SET_AUTORELOAD(&VIBMOT_TIMER, 5000);
	__HAL_TIM_SET_COMPARE(&VIBMOT_TIMER, TIM_CHANNEL_4, (int)map(freq, 260, 500, 2000, 5000));
	HAL_TIM_PWM_Start(&BUZZER_TIMER, TIM_CHANNEL_3);
	__HAL_TIM_SET_AUTORELOAD(&BUZZER_TIMER, 1000000/freq);
	__HAL_TIM_SET_COMPARE(&BUZZER_TIMER, TIM_CHANNEL_3, 1000000/freq/2);
	MHAL_Delay(duration);
	HAL_TIM_PWM_Stop(&BUZZER_TIMER, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&VIBMOT_TIMER, TIM_CHANNEL_4);
	MHAL_Delay((int)(duration*0.3));
}

void beeb_beeb(System* sys, int freq, int duration){
	if(getSoundEnable(sys) && getVibEnable(sys))
		set_tone_vib(freq, duration);
	else if(getSoundEnable(sys) && (!getVibEnable(sys)))
		set_tone(freq, duration);
	else if((!getSoundEnable(sys)) && getVibEnable(sys))
		set_vib(freq, duration);
}

void MHAL_Delay(uint32_t Delay){
	HAL_TIM_Base_Start(&MDELAY_TIMER);
	while(MDELAY_TIMER_VALUE < Delay);
	HAL_TIM_Base_Stop(&MDELAY_TIMER);
	MDELAY_TIMER_VALUE = 0;
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

