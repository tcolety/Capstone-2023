/*
 * gpio.c
 *
 *  Created on: Feb 16, 2023
 *      Author: tcole
 */

#include "gpio.h"
#include "voa_comm.h"
#include "scheduler.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define NP_GPIO_PINS	(1<<1) | (1<<2) | (1<<7) | (1<<12)

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
uint16_t pin;
uint8_t np_pressed;

/* Private user code ---------------------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

/**
  * @brief GPIO Interrupt Handler
  * @param GPIO_Pin indicates which pin triggered interrupt
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == (0x1<<4)){
		/* VOA Busy Bit Interrupt */
//		voa_busy();
	} else if(GPIO_Pin | NP_GPIO_PINS){
		/* Number Pad Interrupt */
//		HAL_NVIC_DisableIRQ(EXTI1_IRQn);
//		HAL_NVIC_DisableIRQ(EXTI2_IRQn);
//		HAL_NVIC_DisableIRQ(EXTI0_IRQn);
//		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		np_pressed = 1;
		pin = GPIO_Pin;
		add_scheduled_event(NUMBER_PAD_CB);
	} else {
		pin = GPIO_Pin;
	}
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB12 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_12|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);

}

/**
  * @brief GPIO Initialization Global Function
  * @param None
  * @retval None
  */
void gpio_init(void){
	MX_GPIO_Init();
	np_pressed = 0;
}

/**
  * @brief returns whether number pad has been pressed and needs to be decoded
  * @param None
  * @retval None
  */
uint8_t get_np_pressed(void){
	return np_pressed;
}

uint16_t get_pin_pressed(void){
	return pin;
}

void reset_np_press(void){
	np_pressed = 0;
}
