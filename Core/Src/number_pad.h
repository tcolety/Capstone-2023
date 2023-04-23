/*
 * number_pad.h
 *
 *  Created on: Feb 16, 2023
 *      Author: tcole
 */

#ifndef SRC_NUMBER_PAD_H_
#define SRC_NUMBER_PAD_H_

/* Private includes ----------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
//void number_pad_callback(uint16_t GPIO_Pin);

typedef struct{
	uint8_t keyPressed;
	uint8_t type;
}NUM_PRESSED_TYPEDEF;

typedef struct{
	uint8_t buff[12]; //buffer to be dispayed on touchscreen ex: {2,4,0x2E,5} = 24.5
//	uint8_t type[12]; //type of button pressed (num, dec, enter, delete, clear, min, max, undefined)
	uint8_t recent;
	uint8_t index;
	uint8_t ts_action;
}np_to_ts_struct;

typedef struct{
	uint8_t action;
	float atten;
	uint8_t integer;
	uint8_t dec_place_value; //describes place value of next num typed (1=tenths, 2=hundreths)
	uint8_t dec_value;
}np_to_voa_struct;


enum{
	none,
	set_atten,
	query,
};

enum{
	undefined,
	num,
	dec,
	enter,
	delete,
	clear,
	min,
	max,
};



extern np_to_voa_struct voa_settings;
extern np_to_ts_struct np_buffer;
extern uint8_t buff_to_touchscreen[2];

/* Private defines -----------------------------------------------------------*/
void number_pad_polling(void);
NUM_PRESSED_TYPEDEF* number_pad_read(uint16_t GPIO_Pin);
float buffer_to_float(void);
void add_to_buffer(uint8_t num);
void number_pad_init(void);
void reset_np_buffer(void);
void voa_query_request(void);
void set_min_atten(void);
void set_max_atten(void);



#endif /* SRC_NUMBER_PAD_H_ */
