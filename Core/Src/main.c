/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
UART_Packet touchscreenInputs;
static void MX_NVIC_Init(void);

/* Private user code ---------------------------------------------------------*/


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	uint8_t i2cBusy = 0;
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  gpio_init();
  UART_init();
//  MX_NVIC_Init();
  scheduler_open();
  voa_comm_init();
  number_pad_init();
  uart_receive_init();
  touchscreen_init();
//  init_voa();


  /* Infinite loop */
  while (1)
  {
//	  i2cBusy = checkI2CBusBusy();
	  while((get_scheduled_events() != 0)|(i2cBusy == 1)){
		  if(get_scheduled_events() & AARDVARK_CB){
			remove_scheduled_event(AARDVARK_CB);
			lockTouchscreen();
			lockDevice();
//			aardvark_callback();
			break;
		  }

		  if(get_scheduled_events() & TOUCHSCREEN_RECEIVE_CB){
			remove_scheduled_event(TOUCHSCREEN_RECEIVE_CB);
			touchscreen_receive_callback();
			break;
		  }

		  if(getDeviceLockStatus() == 0){
				if(get_scheduled_events() & NUMBER_PAD_CB){
					remove_scheduled_event(NUMBER_PAD_CB);
					number_pad_cb();
					break;
				}

				if(get_scheduled_events() & TOUCHSCREEN_TRANSMIT_CB){
					remove_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
					touchscreen_transmit_cb();
					break;
				}

				if(get_scheduled_events() & SET_ATTEN_CB){
					remove_scheduled_event(SET_ATTEN_CB);
					set_atten_callback();
					break;
				}

				if(get_scheduled_events() & CHECK_ATTEN_CB){
					remove_scheduled_event(CHECK_ATTEN_CB);
					check_atten_callback();
					break;
				}

				if(get_scheduled_events() & TOUCHSCREEN_SELECT_CB){
					remove_scheduled_event(TOUCHSCREEN_SELECT_CB);
					selectChannel();
					break;
				}
			  }
		  }
	  }

//	  set_attenuation(0);
//	  query_attenuation();
//	  set_attenuation(30);
//	  query_attenuation();
//	  for(int i=0; i<100000; i++){}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

///**
//  * @brief NVIC Configuration.
//  * @retval None
//  */
//static void MX_NVIC_Init(void)
//{
//  /* USART2_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(USART2_IRQn);
//}


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
