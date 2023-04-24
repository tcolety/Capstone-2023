/*
 * app.h
 *
 *  Created on: Apr 4, 2023
 *      Author: tcole
 */

#ifndef SRC_APP_H_
#define SRC_APP_H_

/* Private includes ----------------------------------------------------------*/
#include "UART.h"
#include "voa_comm.h"
#include "number_pad.h"
#include "scheduler.h"
#include "gpio.h"

/* Private defines ----------------------------------------------------------*/
#define INPUT_NUM_LENGTH		5
#define NUM_OF_VOAS				12

#define INPUT_FIELD_T			0

#define ALL_VOAS					0b111111111111


/* Private structs ----------------------------------------------------------*/
typedef struct{
	uint8_t V1[INPUT_NUM_LENGTH];
	uint8_t V2[INPUT_NUM_LENGTH];
	uint8_t V3[INPUT_NUM_LENGTH];
	uint8_t V4[INPUT_NUM_LENGTH];
	uint8_t V5[INPUT_NUM_LENGTH];
	uint8_t V6[INPUT_NUM_LENGTH];
	uint8_t V7[INPUT_NUM_LENGTH];
	uint8_t V8[INPUT_NUM_LENGTH];
	uint8_t V9[INPUT_NUM_LENGTH];
	uint8_t V10[INPUT_NUM_LENGTH];
	uint8_t V11[INPUT_NUM_LENGTH];
	uint8_t V12[INPUT_NUM_LENGTH];
	uint8_t arr_size[NUM_OF_VOAS];
	float curr_atten[NUM_OF_VOAS];
	uint16_t update_voa_atten; //bit it high to show which VOA still needs to be set
	uint16_t update_display_atten; //bit is high when displayed VOA channel needs to be updated
}CURR_VOA_INFO_TYPEDEF;


void app_init(void);
void set_atten_callback(void);
void check_atten_callback(void);
void number_pad_cb(void);
void touchscreen_transmit_cb(void);
void touchscreen_init(void);
uint8_t getDeviceLockStatus(void);
void lockDevice(void);
void unlockDevice(void);

#endif /* SRC_APP_H_ */
