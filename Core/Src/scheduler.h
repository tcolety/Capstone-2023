/*
 * scheduler.h
 *
 *  Created on: Mar 14, 2023
 *      Author: tcole
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include <stdint.h>

// EVENT FLAGS
#define NULL_CB						0x00000000
#define NUMBER_PAD_CB				0x00000001
#define TOUCHSCREEN_RECEIVE_CB		0x00000002
#define TOUCHSCREEN_TRANSMIT_CB		0x00000004
#define AARDVARK_CB					0x00000008
#define SET_ATTEN_CB				0x00000010
#define CHECK_ATTEN_CB				0x00000020
#define TOUCHSCREEN_SELECT_CB		0x00000040


void scheduler_open(void);
void add_scheduled_event(uint32_t event);
void remove_scheduled_event(uint32_t event);
uint32_t get_scheduled_events(void);


#endif /* SRC_SCHEDULER_H_ */
