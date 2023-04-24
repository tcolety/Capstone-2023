/*
 * voa_comm.h
 *
 *  Created on: Jan 19, 2023
 *      Author: tcole
 */

#ifndef SRC_VOA_COMM_H_
#define SRC_VOA_COMM_H_

#include "stm32f1xx_hal.h"

//#define VOA_1_ADDR 0xC8

#define VOA1_ADDR					0x64<<1
#define VOA2_ADDR					0x65<<1
#define VOA3_ADDR					0x66<<1
#define VOA4_ADDR					0x67<<1
#define VOA5_ADDR					0x68<<1
#define VOA6_ADDR					0x69<<1
#define VOA7_ADDR					0x6a<<1
#define VOA8_ADDR					0x6b<<1
#define VOA9_ADDR					0x6c<<1
#define VOA10_ADDR					0x6d<<1
#define VOA11_ADDR					0x6e<<1
#define VOA12_ADDR					0x6f<<1

#define VOA1_FLAG					0b000000000001
#define VOA2_FLAG					0b000000000010
#define VOA3_FLAG					0b000000000100
#define VOA4_FLAG					0b000000001000
#define VOA5_FLAG					0b000000010000
#define VOA6_FLAG					0b000000100000
#define VOA7_FLAG					0b000001000000
#define VOA8_FLAG					0b000010000000
#define VOA9_FLAG					0b000100000000
#define VOA10_FLAG					0b001000000000
#define VOA11_FLAG					0b010000000000
#define VOA12_FLAG					0b100000000000


void voa_comm_init(void);
void init_voa(void);
void aardvark_callback(void);
void voa_busy(void);
float query_attenuation(void);
void set_attenuation(uint16_t voaCH,float dB);

#endif /* SRC_VOA_COMM_H_ */

