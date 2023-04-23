/*
 * number_pad.c
 *
 *  Created on: Feb 16, 2023
 *      Author: tcole
 */

/* Private includes ----------------------------------------------------------*/
#include "number_pad.h"
#include "gpio.h"
#include "UART.h"
#include "scheduler.h"

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructPrivate = {0};

/* Private define ------------------------------------------------------------*/
#define DEC_BUTTON			0xE
#define MIN_VALUE			0
#define MAX_VALUE_TS_P1		3
#define MAX_VALUE_TS_P2		0
#define MAX_VALUE           30

/* Private macro -------------------------------------------------------------*/
//struct np_to_ts_struct{
//	uint8_t buff[12];
//	uint8_t type[12];
//	uint8_t recent;
//	uint8_t index;
//};

/* Private variables ---------------------------------------------------------*/

uint8_t buff_to_touchscreen[2];
uint8_t set;
uint8_t outpin;
uint32_t startTime, currTime1, currTime2, currTime3, currTime4;
uint32_t previousMillis = 0;
uint32_t currentMillis = 0;
uint8_t keyPressed;
np_to_ts_struct np_buffer;
np_to_voa_struct voa_settings;
uint8_t in_range;
NUM_PRESSED_TYPEDEF num_pressed;

/* Private function prototypes -----------------------------------------------*/
void delete_num(void);
void reset_voa_setting_buff(void);
/* Private user code ---------------------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

void add_to_buffer(uint8_t num){
	float temp = (float)num;
	uint8_t decPlaceValue = voa_settings.dec_place_value;
	if(in_range == 1)
	{
		if(num == DEC_BUTTON)						// checks if decimal was typed
		{
			if(decPlaceValue == 0){
				np_buffer.buff[np_buffer.index] = 0x2E;  // ascii "." added to buffer (touchscreen format)
				np_buffer.index++;
				voa_settings.dec_place_value++;			 // describes the place value of next num typed (1=tenths)
			}
		}
		else
		{
			// CODE TO UPDATE VOA FORMAT ARRAY
			if(decPlaceValue == 0)						// num is integer place value, not decimal
			{
				voa_settings.integer = voa_settings.integer*10 + num;
				if(voa_settings.integer > 30.0)
				{
					in_range = 0;
				}
				else
				{
					voa_settings.atten = (float)voa_settings.integer;
				}

			}
			else
			{
				for(int i=0; i<decPlaceValue; i++)
				{
					temp = temp/10.0;
				}
	//			voa_settings.dec_value = voa_settings.dec_value + temp;
				voa_settings.atten =(float)voa_settings.atten + temp;
				voa_settings.dec_place_value++;
			}
			// CODE TO UPDATE TOUCHSCREEN FORMAT ARRAY
			if(in_range ==1)
			{
				if((np_buffer.index == 1) && (np_buffer.buff[0] == 0)) 	//if first and only elem is 0, then replace with new num
				{													   	//ex: '0' -> '8' instead of '08'
					np_buffer.buff[0] = num;
				}
				else													//num is added to np_buffer
				{
					np_buffer.buff[np_buffer.index] = num;
					np_buffer.index++;
				}
			}
			in_range = 1; 								// resets in_range for next num typed
		}
	}
}

void number_pad_init(void){
	reset_np_buffer();
	set = 1;
	keyPressed = 0;
	voa_settings.action = none;
	voa_settings.atten = 0.0;
	voa_settings.integer = 0;
	voa_settings.dec_place_value = 0;
	voa_settings.dec_value = 0;
	in_range = 1;
}

void reset_np_buffer(void){
	for(int i =0; i<12; i++){
		np_buffer.buff[i] = 0;
	}
	np_buffer.index = 1;
	in_range = 1;
}

void reset_voa_setting_buff(void){
	voa_settings.dec_place_value = 0;
	voa_settings.dec_value = 0;
	voa_settings.integer = 0;
	voa_settings.action = none;
	voa_settings.atten = 0;
}


void delete_num(void){
	uint8_t num = np_buffer.buff[np_buffer.index-1];
	float temp = (float) num;
	uint8_t decPlaceValue = voa_settings.dec_place_value;
	// code to delete num in voa_settings
	if(decPlaceValue == 0)		//deleted num is an integer
	{
		voa_settings.integer = (voa_settings.integer - num) / 10.0;
		voa_settings.atten = (float)voa_settings.integer;
	}
	else if(decPlaceValue == 1)							//delete num is decimal point
	{
		voa_settings.dec_place_value = 0;
	}
	else
	{
		decPlaceValue = decPlaceValue-1;
		for(int i=0; i<decPlaceValue; i++)				// deleted num is a decimal
		{
			temp = temp/10.0;
		}
		voa_settings.atten =(float)voa_settings.atten - temp;
		voa_settings.dec_place_value = decPlaceValue;
	}

	//code to delete num in np_buffer
	np_buffer.index = np_buffer.index - 1;
	np_buffer.buff[np_buffer.index] = 0;
	if(np_buffer.index == 0)
	{								  //if arr empty, set index=1 to display 0
		np_buffer.index = 1;
	}
}

//void voa_query_request(void){
//	voa_settings.action = query;
//}

void set_min_atten(void){
	reset_np_buffer();
	reset_voa_setting_buff();
	add_to_buffer(MIN_VALUE);
}

void set_max_atten(void){
	reset_np_buffer();
	reset_voa_setting_buff();
	add_to_buffer(MAX_VALUE_TS_P1);
	add_to_buffer(MAX_VALUE_TS_P2);
}

/**
  * @brief Intreprets number that's been pressed
  * @param None
  * @retval None
  */
NUM_PRESSED_TYPEDEF* number_pad_read(uint16_t GPIO_Pin){
	uint8_t number = undefined;
	currentMillis = HAL_GetTick();
	  if (currentMillis - previousMillis > 10) {

	//interpret button pressed
//	HAL_GPIO_DeInit(GPIOB, NP_INPUT_PIN1|NP_INPUT_PIN2|NP_INPUT_PIN3|NP_INPUT_PIN4);

	/*Configure GPIO pins : PB6 PB7 PB8 PB9 to GPIO_INPUT*/
	GPIO_InitStructPrivate.Pin = NP_INPUT_PIN1|NP_INPUT_PIN2|NP_INPUT_PIN3|NP_INPUT_PIN4;
	GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
	GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructPrivate);

	HAL_GPIO_WritePin(NP_OUTPUT_PORT1, NP_OUTPUT_PIN1, 1);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT2, NP_OUTPUT_PIN2, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT3, NP_OUTPUT_PIN3, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT4, NP_OUTPUT_PIN4, 0);
	if(GPIO_Pin == NP_INPUT_PIN1 && HAL_GPIO_ReadPin(NP_INPUT_PORT1, NP_INPUT_PIN1))
	{
	  keyPressed = 1; //ASCII value of D
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN2 && HAL_GPIO_ReadPin(NP_INPUT_PORT2, NP_INPUT_PIN2))
	{
	  keyPressed = 2; //ASCII value of C
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN3 && HAL_GPIO_ReadPin(NP_INPUT_PORT3, NP_INPUT_PIN3))
	{
	  keyPressed = 3; //ASCII value of B
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN4 && HAL_GPIO_ReadPin(NP_INPUT_PORT4, NP_INPUT_PIN4))
	{
	  keyPressed = 0xA; //ASCII value of A
	  number = min;
	}

	HAL_GPIO_WritePin(NP_OUTPUT_PORT1, NP_OUTPUT_PIN1, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT2, NP_OUTPUT_PIN2, 1);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT3, NP_OUTPUT_PIN3, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT4, NP_OUTPUT_PIN4, 0);
	if(GPIO_Pin == NP_INPUT_PIN1 && HAL_GPIO_ReadPin(NP_INPUT_PORT1, NP_INPUT_PIN1))
	{
	  keyPressed = 4; //ASCII value of D
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN2 && HAL_GPIO_ReadPin(NP_INPUT_PORT2, NP_INPUT_PIN2))
	{
	  keyPressed = 5; //ASCII value of C
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN3 && HAL_GPIO_ReadPin(NP_INPUT_PORT3, NP_INPUT_PIN3))
	{
	  keyPressed = 6; //ASCII value of B
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN4 && HAL_GPIO_ReadPin(NP_INPUT_PORT4, NP_INPUT_PIN4))
	{
	  keyPressed = 0xB; //ASCII value of A
	  number = max;
	}

	HAL_GPIO_WritePin(NP_OUTPUT_PORT1, NP_OUTPUT_PIN1, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT2, NP_OUTPUT_PIN2, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT3, NP_OUTPUT_PIN3, 1);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT4, NP_OUTPUT_PIN4, 0);
	if(GPIO_Pin == NP_INPUT_PIN1 && HAL_GPIO_ReadPin(NP_INPUT_PORT1, NP_INPUT_PIN1))
	{
	  keyPressed = 7; //ASCII value of D
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN2 && HAL_GPIO_ReadPin(NP_INPUT_PORT2, NP_INPUT_PIN2))
	{
	  keyPressed = 8; //ASCII value of C
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN3 && HAL_GPIO_ReadPin(NP_INPUT_PORT3, NP_INPUT_PIN3))
	{
	  keyPressed = 9; //ASCII value of B
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN4 && HAL_GPIO_ReadPin(NP_INPUT_PORT4, NP_INPUT_PIN4))
	{
	  keyPressed = 0xC; //ASCII value of A
	  number = clear;
	}

	HAL_GPIO_WritePin(NP_OUTPUT_PORT1, NP_OUTPUT_PIN1, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT2, NP_OUTPUT_PIN2, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT3, NP_OUTPUT_PIN3, 0);
	HAL_GPIO_WritePin(NP_OUTPUT_PORT4, NP_OUTPUT_PIN4, 1);
	if(GPIO_Pin == NP_INPUT_PIN1 && HAL_GPIO_ReadPin(NP_INPUT_PORT1, NP_INPUT_PIN1))
	{
	  keyPressed = 0xE; //ASCII value of D
	  number = dec;
	}
	else if(GPIO_Pin == NP_INPUT_PIN2 && HAL_GPIO_ReadPin(NP_INPUT_PORT2, NP_INPUT_PIN2))
	{
	  keyPressed = 0; //ASCII value of C
	  number = num;
	}
	else if(GPIO_Pin == NP_INPUT_PIN3 && HAL_GPIO_ReadPin(NP_INPUT_PORT3, NP_INPUT_PIN3))
	{
	  keyPressed = 0xF; //ASCII value of B
	  number = delete;
	}
	else if(GPIO_Pin == NP_INPUT_PIN4 && HAL_GPIO_ReadPin(NP_INPUT_PORT4, NP_INPUT_PIN4))
	{
	  keyPressed = 0xD; //ASCII value of A
	  number = enter;
	}
	GPIO_InitStructPrivate.Pin = NP_INPUT_PIN1|NP_INPUT_PIN2|NP_INPUT_PIN3|NP_INPUT_PIN4;
	GPIO_InitStructPrivate.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructPrivate.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructPrivate);

	previousMillis = currentMillis;
  } else {
	  number = undefined;
	  keyPressed = 0;

  }
  reset_np_press();
  HAL_GPIO_WritePin(NP_OUTPUT_PORT1, NP_OUTPUT_PIN1, 1);
  HAL_GPIO_WritePin(NP_OUTPUT_PORT2, NP_OUTPUT_PIN2, 1);
  HAL_GPIO_WritePin(NP_OUTPUT_PORT3, NP_OUTPUT_PIN3, 1);
  HAL_GPIO_WritePin(NP_OUTPUT_PORT4, NP_OUTPUT_PIN4, 1);

//  NUM_PRESSED_TYPEDEF num_pressed;
  num_pressed.type = number;
  num_pressed.keyPressed = keyPressed;
  return &num_pressed;

}


