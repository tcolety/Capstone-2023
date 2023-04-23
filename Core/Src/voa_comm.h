/*
 * voa_comm.h
 *
 *  Created on: Jan 19, 2023
 *      Author: tcole
 */

#ifndef SRC_VOA_COMM_H_
#define SRC_VOA_COMM_H_

#include "stm32f1xx_hal.h"

#define VOA_1_ADDR 0xC8




void voa_comm_init(void);
void init_voa(void);
void aardvark_callback(void);
void voa_busy(void);
float query_attenuation(void);
void set_attenuation(float value);

#endif /* SRC_VOA_COMM_H_ */

