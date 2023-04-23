/*

UART.h*
Created on: Feb 16, 2023
Author: tcole
*/

#ifndef SRC_UARTH
#define SRC_UARTH

#include "stm32f1xx_hal.h"
#include "scheduler.h"
#include "app.h"



// Defines the Frame tail and header for MCU->Touchscreen communication


void UART_init(void);
void np_to_touchscreen(void);
void touchscreen_update(uint8_t *cmdCode,uint8_t *type,uint8_t *widgit, uint8_t* value, uint8_t valueArrSize, uint8_t toggleText);
// UART Packet Received Structure

typedef struct{
    uint8_t FrameHeader[3];
    uint8_t CMD[2];
    uint8_t LEN[2];
    uint8_t Data[4];
    uint8_t FrameTail[3];
    uint8_t Verification[2];

}UART_Packet;

extern UART_Packet touchscreenInputs; // global Variable for the inputs to the transmit function

extern uint8_t TxData[100];
extern volatile UART_Packet Rx_Buffer;
extern UART_HandleTypeDef huart2;
extern uint8_t voaSelect[14];


UART_Packet parseUART(void);
void UART_Init(void);
void uart_receive_init(void);
void clearRxData(void);
void parseTouchIn(UART_Packet tsPacket);
void addChannel(uint8_t voaCH);
void clearVOA(void);
uint16_t getVOA(void);
void touchscreen_receive_callback(void);
void update_display_channel(uint8_t VOA, uint8_t* atten, uint8_t size);
void lockTouchscreen(void);
void removeVOA(uint8_t voa);
uint8_t isIn(uint8_t voaNum);
void selectChannel(void);
void deselectVOA(void);

#endif /* SRC_UARTH */
