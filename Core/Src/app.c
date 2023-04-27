/*
 * app.c
 *
 *  Created on: Apr 4, 2023
 *      Author: tcole
 */

/* Includes ------------------------------------------------------------------*/
#include "app.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/
enum {
	INPUT_FIELD,
	UPDATE_ALL_CH,
};

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
CURR_VOA_INFO_TYPEDEF curr_voa_info;
uint8_t touchscreen_state;
uint8_t deviceLock;

/* Private function prototypes -----------------------------------------------*/
void update_curr_voa_info(void);
uint8_t get_update_VOA(void);
uint8_t *get_VOA_atten_arr(uint8_t VOA);
void clear_voa_update_flag(uint8_t VOA);


/* Private user code ---------------------------------------------------------*/
void update_curr_voa_info(void){
	float atten_local = voa_settings.atten;
	uint16_t voa_selected = getVOA();
	uint8_t *voa_value_arr;
	for(int i=0; i<NUM_OF_VOAS; i++){
		if(voa_selected & (1<<i)){
			voa_value_arr = get_VOA_atten_arr(i+1);
			curr_voa_info.arr_size[i] = np_buffer.index;
			curr_voa_info.curr_atten[i] = atten_local;
			for(int j=0; j<INPUT_NUM_LENGTH; j++){
				voa_value_arr[j] = np_buffer.buff[j];
			}
		}
	}
	clearVOA();
	curr_voa_info.update_voa_atten = curr_voa_info.update_voa_atten|voa_selected;
	curr_voa_info.update_display_atten = ALL_VOAS;
}

void number_pad_cb(void){
	NUM_PRESSED_TYPEDEF key = *number_pad_read(get_pin_pressed());
	//if statement to decide which function to call:
	switch(key.type){
		case num:
			add_to_buffer(key.keyPressed);
			add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
			break;
		case dec:
			add_to_buffer(key.keyPressed);
			add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
			break;
		case enter:
			update_curr_voa_info();
			reset_np_buffer();
			reset_voa_setting_buff();
			touchscreen_state = UPDATE_ALL_CH;
			add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
			add_scheduled_event(SET_ATTEN_CB);
//			selectAllVOA(0);   //dehighlights voas
			break;
		case delete:
			delete_num();
			add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
			break;
		case clear:
			reset_np_buffer();
			reset_voa_setting_buff();
			add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
			break;
		case min:
			set_min_atten();
			add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
			break;
		case max:
			set_max_atten();
			add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
			break;
	}
}


void set_atten_callback(void){
	uint8_t check_atten = 0; //bool to tell if anymore voas need to be set; if not, then check atten
	for(int i=0; i<NUM_OF_VOAS; i++){
		if(curr_voa_info.update_voa_atten & (1<<i)){
			set_attenuation((1<<i), curr_voa_info.curr_atten[i]);
			curr_voa_info.update_voa_atten = curr_voa_info.update_voa_atten & ~(1<<i);
			check_atten=1;
			add_scheduled_event(SET_ATTEN_CB);
			break;
		}
	}
	if(check_atten)	add_scheduled_event(CHECK_ATTEN_CB);

}

void check_atten_callback(void){
//	if(voa_settings.action == query){
		float check_atten = query_attenuation();				// atten read from voa
		check_atten = query_attenuation();
		float set_atten = ((uint32_t)(curr_voa_info.curr_atten[0]*100))&0xff;  // atten previously set
		if(check_atten != set_atten){
//			Error_Handler();
		}
//	}
}

void touchscreen_transmit_cb(void){
	uint8_t VOA;
	uint8_t *atten_value;
	switch(touchscreen_state){
	case INPUT_FIELD:
		update_display_channel(INPUT_FIELD_T, np_buffer.buff, np_buffer.index);
		break;
	case UPDATE_ALL_CH:
		VOA = get_update_VOA();
		atten_value = get_VOA_atten_arr(VOA);
		update_display_channel(VOA, atten_value, curr_voa_info.arr_size[VOA-1]);
		clear_voa_update_flag(VOA);
		if(curr_voa_info.update_display_atten == 0){
			touchscreen_state = INPUT_FIELD;
		}
		add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
		break;
	}
}

void touchscreen_init(void){
	for(int i=0; i<INPUT_NUM_LENGTH; i++){
		curr_voa_info.V1[i] = 0;
		curr_voa_info.V2[i] = 0;
		curr_voa_info.V3[i] = 0;
		curr_voa_info.V4[i] = 0;
		curr_voa_info.V5[i] = 0;
		curr_voa_info.V6[i] = 0;
		curr_voa_info.V7[i] = 0;
		curr_voa_info.V8[i] = 0;
		curr_voa_info.V9[i] = 0;
		curr_voa_info.V10[i] = 0;
		curr_voa_info.V11[i] = 0;
		curr_voa_info.V12[i] = 0;
	}
	for(int i=0; i<NUM_OF_VOAS; i++){
		curr_voa_info.curr_atten[i] = 0;
		curr_voa_info.arr_size[i] = 1;
	}
	curr_voa_info.update_voa_atten = 0b000000000000;
	curr_voa_info.update_display_atten = ALL_VOAS;
	touchscreen_state = UPDATE_ALL_CH;
	add_scheduled_event(TOUCHSCREEN_TRANSMIT_CB);
//	add_scheduled_event(TOUCHSCREEN_SELECT_CB);
	deviceLock = 0;
}

uint8_t get_update_VOA(void){
	for(int i=0; i<NUM_OF_VOAS; i++){
		if(curr_voa_info.update_display_atten & (1<<i)){
			return i+1;
		}
	}
	return 0;
}

uint8_t *get_VOA_atten_arr(uint8_t VOA){
	uint8_t *ptr_VOA_arr;
	switch(VOA){
	case 1:
		ptr_VOA_arr = curr_voa_info.V1;
		break;
	case 2:
		ptr_VOA_arr = curr_voa_info.V2;
		break;
	case 3:
		ptr_VOA_arr = curr_voa_info.V3;
		break;
	case 4:
		ptr_VOA_arr = curr_voa_info.V4;
		break;
	case 5:
		ptr_VOA_arr = curr_voa_info.V5;
		break;
	case 6:
		ptr_VOA_arr = curr_voa_info.V6;
		break;
	case 7:
		ptr_VOA_arr = curr_voa_info.V7;
		break;
	case 8:
		ptr_VOA_arr = curr_voa_info.V8;
		break;
	case 9:
		ptr_VOA_arr = curr_voa_info.V9;
		break;
	case 10:
		ptr_VOA_arr = curr_voa_info.V10;
		break;
	case 11:
		ptr_VOA_arr = curr_voa_info.V11;
		break;
	case 12:
		ptr_VOA_arr = curr_voa_info.V12;
		break;
	default:
		Error_Handler();
	}
	return ptr_VOA_arr;
}

void clear_voa_update_flag(uint8_t VOA){
	uint16_t voa_flag = (1<<(VOA-1));
	curr_voa_info.update_display_atten = curr_voa_info.update_display_atten & ~(voa_flag);
}

void unlockDevice(void){
	deviceLock = 0;
}

void lockDevice(void){
	deviceLock = 1;
}

uint8_t getDeviceLockStatus(void){
	return deviceLock;
}
