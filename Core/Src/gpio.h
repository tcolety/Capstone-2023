/*
 * gpio.h
 *
 *  Created on: Feb 16, 2023
 *      Author: tcole
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

/* Private includes ----------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "voa_comm.h"
#include "number_pad.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void gpio_init(void);
uint8_t get_np_pressed(void);
uint16_t get_pin_pressed(void);
void reset_np_press(void);

/* Private defines -----------------------------------------------------------*/
// NUMBER PAD OUTPUT PINS
#define		NP_OUTPUT_PIN1		GPIO_PIN_13
#define		NP_OUTPUT_PORT1		GPIOB
#define		NP_OUTPUT_PIN2		GPIO_PIN_14
#define		NP_OUTPUT_PORT2		GPIOB
#define		NP_OUTPUT_PIN3		GPIO_PIN_15
#define		NP_OUTPUT_PORT3		GPIOB
#define		NP_OUTPUT_PIN4		GPIO_PIN_8
#define		NP_OUTPUT_PORT4		GPIOA

// NUMBER PAD INTERRUPT PINS
#define		NP_INPUT_PIN1		GPIO_PIN_1
#define		NP_INPUT_PORT1		GPIOB
#define		NP_INPUT_PIN2		GPIO_PIN_0
#define		NP_INPUT_PORT2		GPIOB
#define		NP_INPUT_PIN3		GPIO_PIN_7
#define		NP_INPUT_PORT3		GPIOB
#define		NP_INPUT_PIN4		GPIO_PIN_12
#define		NP_INPUT_PORT4		GPIOB

// BUSY BIT PINS

// UART PINS


#endif /* SRC_GPIO_H_ */
