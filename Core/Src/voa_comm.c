/*
 * voa_comm.c
 *
 *  Created on: Jan 19, 2023
 *      Author: tcole
 */

#include "voa_comm.h"
#include "number_pad.h"
#include "scheduler.h"
/* Private includes ----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BUFFER_LENGTH			5
#define STOP_FALSE				0
#define STOP_TRUE				1


/* Private typedef -----------------------------------------------------------*/
struct data{
	uint8_t received;
	uint8_t send[BUFFER_LENGTH];
	uint8_t receive[BUFFER_LENGTH];
	uint8_t size;
} i2c_data;

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
uint8_t setting_voa_check;
uint8_t touchscreen_input_enabled;
uint8_t recent_atten;

enum voa_setting{
	set,
	flagged,
	done,
	aardvark_started,
	aardvark_done,
};

enum touchscreen_enable{
	enable,
	disable,
};

/* Private function prototypes -----------------------------------------------*/
static void MX_I2C1_Init(void);
void i2c_receive(uint8_t address, uint8_t *data, uint8_t size);
void i2c_transmit(uint8_t address, uint8_t *send_data, uint8_t size, uint8_t stop_true);
void reset_buffer(void);
uint8_t get_VOA_ADDR(uint16_t voaCH);


/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 10000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void voa_comm_init(void){
	MX_I2C1_Init();
	i2c_data.size = 0;
	reset_buffer();
	setting_voa_check = done;
}

void reset_buffer(void){
	for(int i=0; i<BUFFER_LENGTH; i++){
	  i2c_data.send[i] = 0;
	  i2c_data.receive[i] = 0;
	}
}

void init_voa(void) {
  //send and receive data has depth of 12 (for multiple bytes?)

  //Construct send package (write)
  //to_slave holds return of the master transmit
  i2c_data.send[0] = 0x8D;
  i2c_data.size = 1;
  i2c_transmit(VOA1_ADDR, i2c_data.send, i2c_data.size,STOP_FALSE);

  //Construct receive package (read)
  //to_slave holds return of the master transmit
  i2c_data.size = 3;
  i2c_receive(VOA1_ADDR, i2c_data.receive, i2c_data.size);

  i2c_data.send[0] = 0;

  reset_buffer();
  set_attenuation(1,5);
  query_attenuation();
  set_attenuation(1,15);
    query_attenuation();
}

void voa_busy(void){
	if(setting_voa_check == set){
		setting_voa_check = flagged;
	} else if (setting_voa_check == flagged){
		setting_voa_check = done;
	} else if(setting_voa_check == done){
		setting_voa_check = aardvark_started;
		add_scheduled_event(AARDVARK_CB);
	}
}

void aardvark_callback(void){
	//disable all i2c and cancel all comm
	while(1){

	}

	//send something to touchscreen to disable use

	//wait til line not busy, then query VOA

}

void set_attenuation(uint16_t voaCH, float dB){
	reset_buffer();
	uint32_t value = (uint32_t)(dB * 100);
	i2c_data.received = dB;
	if(value & (0xFFFF0000)){
		//out of range, tell touchscreen
		Error_Handler();
	}
	i2c_data.send[0] = 0x80;
	i2c_data.send[1] = (value & (0xFF00)) >> 8;
	i2c_data.send[2] = value & (0xFF);
	recent_atten = i2c_data.send[2];
	i2c_data.size = 3;
	setting_voa_check = set;
	i2c_transmit(get_VOA_ADDR(voaCH), i2c_data.send, i2c_data.size,STOP_TRUE);
	return;
}

float query_attenuation(void){
	float dB;
	reset_buffer();
	i2c_data.send[0] = 0x81;
	i2c_data.size = 1;
	i2c_transmit(VOA1_ADDR, i2c_data.send, i2c_data.size,STOP_TRUE);
	i2c_data.size = 2;
	i2c_receive(VOA1_ADDR, i2c_data.receive, i2c_data.size);
//	dB = (float)((i2c_data.receive[0] << 8) | i2c_data.receive[1]);
//	dB = dB / 100.0;
	return i2c_data.receive[1];
}

void device_id(void){
	return;
}

void i2c_transmit(uint8_t address, uint8_t *send_data, uint8_t size, uint8_t stop_true){
	HAL_StatusTypeDef to_slave;
	char error;
//	HAL_NVIC_DisableIRQ(EXTI2_IRQn);

	to_slave = HAL_I2C_Master_Transmit(&hi2c1,address,i2c_data.send,size,HAL_MAX_DELAY);
//	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	//check proper write
	if(to_slave != HAL_OK) {
		//throw error
//		Error_Handler();
		error = 1;
	}
}

void i2c_receive(uint8_t address, uint8_t *data, uint8_t size){
	HAL_StatusTypeDef from_slave;
	uint8_t r_data[12];
	for(int i=0; i<12; i++){
	  r_data[i] = 0;
	}
	char error;
//	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	from_slave = HAL_I2C_Master_Receive(&hi2c1,address,r_data,size,HAL_MAX_DELAY);
//	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	//compare expected receive results
	if(from_slave != HAL_OK) {
		//throw error
//		Error_Handler();
	}
	for(int i=0; i<size; i++){
		i2c_data.receive[i] = r_data[i];
	}
}

uint8_t get_VOA_ADDR(uint16_t voaCH){
	uint8_t addr;
	switch(voaCH)
	{
		case VOA1_FLAG: addr=VOA1_ADDR; break;
		case VOA2_FLAG: addr=VOA2_ADDR; break;
		case VOA3_FLAG: addr=VOA3_ADDR; break;
		case VOA4_FLAG: addr=VOA4_ADDR; break;
		case VOA5_FLAG: addr=VOA5_ADDR; break;
		case VOA6_FLAG: addr=VOA6_ADDR; break;
		case VOA7_FLAG: addr=VOA7_ADDR; break;
		case VOA8_FLAG: addr=VOA8_ADDR; break;
		case VOA9_FLAG: addr=VOA9_ADDR; break;
		case VOA10_FLAG: addr=VOA10_ADDR; break;
		case VOA11_FLAG: addr=VOA11_ADDR; break;
		case VOA12_FLAG: addr=VOA12_ADDR; break;
		default: addr=0;
	}
	return addr;
}
