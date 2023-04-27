/*
 * UART.c
 *
 *  Created on: Feb 16, 2023
 *      Author: tcole
 */

#include "UART.h"
#include "number_pad.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
uint32_t NextIndex;
uint32_t PacketLength;
long long touchscreenCMD;
volatile UART_Packet Rx_Buffer;
volatile uint8_t Rx_data[RX_DATA_SIZE] = {0};
uint8_t RxDataIndex;
volatile uint8_t RxData_temp[RX_DATA_TEMP_SIZE] = {0};
uint8_t TxData[100];
uint8_t voaIndex = 0;
uint8_t voaChannel = 0;
uint8_t voaSelect[14];
uint16_t voaHighlighted;	//flags of which voa's highlight needs updating
uint8_t highlight;	//bool if voaSelect is selected or deselected

volatile UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* Private function prototypes -----------------------------------------------*/
static void MX_USART2_UART_Init(void);

static void MX_DMA_Init(void);

/* Private user code ---------------------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

void UART_Init(void);
void clearTxData(void);
void removeVOA(uint8_t voa);
void clearRxData(void);
void clearRxDataTemp(void);


/**
  * @brief Parse UART Touchscreen Packet
  * @param RecieveData; uint8_t array from Rx Buffer
  * @retval touchscreenPacket parsed into specified packet fields
  */

//ST< {"cmd_code":"set_value","type":"label","widget":"label2","value":5} >ET

void uart_receive_init(void){
//	uint8_t ReceiveData[20];
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, RxData_temp, RX_DATA_TEMP_SIZE);
}


void parseTouchIn(UART_Packet tsPacket)
{
	uint8_t voaChannel = 0;
//	if(tsPacket.LEN == 1)
//	{
//
//	}

//	if(tsPacket.LEN[1] == 3)
//	{
		// Update VOA Channel Selection based on TS input
	// Channels 13 - 14 == Select/ Deselect ALL
	if(tsPacket.Data[1]== '1' && tsPacket.Data[2]== '3')
	{
		voaChannel = 13;
	}
	else if(tsPacket.Data[1]== '1' && tsPacket.Data[2]== '4')
	{
		voaChannel = 14;
	}
	else if(tsPacket.Data[1]== '1' && tsPacket.Data[2]== '5')
	{
		voaChannel = 15; //back button after aardvark comm
	}

	// Channels 10 - 12
	else if(tsPacket.Data[1]== '1' && tsPacket.Data[2]== '0')
	{
		voaChannel = 10;
	}
	else if(tsPacket.Data[1]== '1' && tsPacket.Data[2]== '1')
	{
		voaChannel = 11;
	}
	else if(tsPacket.Data[1]== '1' && tsPacket.Data[2]== '2')
	{
		voaChannel = 12;
	}


	// Channels 1 - 9
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '1')
	{
		voaChannel = 1;
	}
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '2')
	{
		voaChannel = 2;
	}
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '3')
	{
		voaChannel = 3;
	}
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '4')
	{
		voaChannel = 4;
	}
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '5')
	{
		voaChannel = 5;
	}
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '6')
	{
		voaChannel = 6;
	}
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '7')
	{
		voaChannel = 7;
	}
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '8')
	{
		voaChannel = 8;
	}
	else if(tsPacket.Data[1]== '0' && tsPacket.Data[2]== '9')
	{
		voaChannel = 9;
	}
//	}
//	else if(tsPacket.LEN[1] == 4)
//	{
	// Update VOA Channel Selection based on TS input
//	}
	if((voaChannel < 15)&&(voaChannel != 0))
	{
		addChannel(voaChannel); // add channel to VOA list
	}
	else if(voaChannel < 15)
	{
		unlockDevice();
	}
}


uint16_t getVOA(void)
{
	uint16_t returnVOA = 0b0000000000000000;
	for(int i = 0; i < 12; i++)
	{
		// Set bit index of VOA selection and return binary value
		if(voaSelect[i] == 1) returnVOA |= 0b0000000000000001;
		else if(voaSelect[i] == 2) returnVOA |= 0b0000000000000010;
		else if(voaSelect[i] == 3) returnVOA |= 0b0000000000000100;
		else if(voaSelect[i] == 4) returnVOA |= 0b0000000000001000;
		else if(voaSelect[i] == 5) returnVOA |= 0b0000000000010000;
		else if(voaSelect[i] == 6) returnVOA |= 0b0000000000100000;
		else if(voaSelect[i] == 7) returnVOA |= 0b0000000001000000;
		else if(voaSelect[i] == 8) returnVOA |= 0b0000000010000000;
		else if(voaSelect[i] == 9) returnVOA |= 0b0000000100000000;
		else if(voaSelect[i] == 10) returnVOA |= 0b000001000000000;
		else if(voaSelect[i] == 11) returnVOA |= 0b000010000000000;
		else if(voaSelect[i] == 12) returnVOA |= 0b000100000000000;
		else returnVOA |= 0b0000000000000000;
	}
	return returnVOA;
}

uint8_t isIn(uint8_t voaNum)
{
	for(int i = 0; i < 12; i++)
	{
		if(voaSelect[i] == voaNum && voaNum != 13 && voaNum != 14) return 1;
	}
	return 0;
}

void addChannel(uint8_t voaCH)
{
	if(voaCH == 13) // SELECT ALL
	{
		// Clear Array
		for(int i = 0; i < 12; i++)
		{
			voaSelect[i] = 0;
		}
		// Set ALL VOAs
		for(int i = 0; i < 12; i++)
		{
			voaSelect[i] = (uint8_t)i+1;
		}
		voaIndex = 13;
		voaHighlighted = 0b111111111111;
		highlight = 1;
	}
	else if(voaCH == 14) // DE SELECT ALL
	{
		for(int i = 0; i < 12; i++)
		{
			voaSelect[i] = 0;
		}
		voaIndex = 0;
		highlight = 0;
		voaHighlighted = 0b111111111111;
	}
	else
	{
		//

	//	check is in
	//	if in
	//	remove
	//	call selct VOA
		if(isIn(voaCH))
		{
			// Set highlight to false
			highlight = 0;
			removeVOA(voaCH);
		}
		else
		{
			// Set Highlight to true
			highlight = 1;
			voaSelect[voaIndex] = voaCH; // Add channel to global voaSelect Array
			voaIndex++;
		}
		voaHighlighted = (1<<(voaCH-1));
		//select VOA call
	}
	add_scheduled_event(TOUCHSCREEN_SELECT_CB);
}

void removeVOA(uint8_t voa)
{
    uint8_t crawler = 0;
    while(voaSelect[crawler] != voa) crawler++;
    while(crawler != 12)
    {
        voaSelect[crawler] = voaSelect[crawler + 1];
        crawler++;
    }
    voaIndex = voaIndex - 1;
}


// Clears VOA buffer as well as resets index
void clearVOA(void)
{
	for(int i =0; i < 12; i++)
	{
		voaSelect[i] = 0;
	}
	voaIndex = 0;
}

void clearRxDataTemp(void){
	for(int i = 0; i < RX_DATA_TEMP_SIZE; i++)
	{
		RxData_temp[i] = 0;
	}
}

void clearRxData(void){
	for(int i = 0; i < RX_DATA_SIZE; i++)
	{
		Rx_data[i] = 0;
	}
	RxDataIndex = 0;
}

void clearTxData(void){
	for(int i = 0; i < 100; i++)
	{
		TxData[i] = 0;
	}
}

UART_Packet parseUART(void)
{
	volatile UART_Packet touchscreenPacket;
	int crawler = 0;
	while(Rx_data[crawler] != 'v')
	{
		crawler++; // find Data frame
		if(crawler > 15) return;
	}
	touchscreenPacket.Data[0] = Rx_data[crawler]; // V
	crawler++;
	touchscreenPacket.Data[1] = Rx_data[crawler]; // 1- 9
	crawler++;
	touchscreenPacket.Data[2] = Rx_data[crawler]; // 1- 3
	crawler++;
	touchscreenPacket.Data[3] = Rx_data[crawler]; // Button State
//	for(int i = 0; i <= 1; i++)
//	{
//		touchscreenPacket.FrameHeader[i] = ReceiveData[i];
//	}
//	int index = 0;
//	for(int i = 2; i <= 3; i++)
//	{
//		touchscreenPacket.CMD[index] = ReceiveData[i];
//		index++;
//	}
//	index = 0;
//	for(int i = 4; i <= 5; i++)
//	{
//		touchscreenPacket.LEN[index] = ReceiveData[i];
//		index++;
//	}
//	PacketLength = touchscreenPacket.LEN[0] + touchscreenPacket.LEN[1];
//	NextIndex = 6 + PacketLength;
//	index = 0;
//	for(int i = 6; i <= NextIndex; i++)
//	{
//		touchscreenPacket.Data[index] = ReceiveData[i];
//		index++;
//	}
//	index = 0;
//	for(int i = NextIndex + 1; i <= NextIndex + 4; i++)
//	{
//		touchscreenPacket.FrameTail[index] = ReceiveData[i];
//		index++;
//	}
//	index = 0;
//	for(int i = NextIndex + 5; i <= NextIndex + 7; i++)
//	{
//		touchscreenPacket.Verification[index] = ReceiveData[i];
//		index++;
//	}
	return touchscreenPacket;
}

void UART_init(void)
{

	MX_DMA_Init();
	MX_USART2_UART_Init();
	clearVOA();
	RxDataIndex = 0;
	highlight = 0;
	voaSelected = 0b111111111111;
//	selectAllVOA(0);

//	__HAL_UART_CLEAR_OREFLAG(&huart2);
//	__HAL_UART_CLEAR_NEFLAG(&huart2);
//	__HAL_UART_CLEAR_FEFLAG(&huart2);
//	__HAL_UART_DISABLE_IT(&huart2, UART_IT_ERR);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	uint8_t index =RxDataIndex;
    if(huart->Instance == USART2) //check rx event
    {
//        HAL_UART_Receive_DMA(&huart2,Rx_data, 16);
//        HAL_UARTEx_ReceiveToIdle_DMA(&huart, Rx_data, 50);
//        huart2->Instance->CR1 |= USART_CR1_RXNEIE;
    	for(int i=0; i<Size; i++)
		{
			Rx_data[index] = RxData_temp[i];
			index++;
		}
    	RxDataIndex = index;
        add_scheduled_event(TOUCHSCREEN_RECEIVE_CB);
    }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart){
	add_scheduled_event(TOUCHSCREEN_RECEIVE_CB);
}


/**
  * @brief Sends a packet to touch screen after number pad pressed
  * @param cmdCode; Character array for specified touchscreen command
  * @param type; Update type
  * @param widget; Specified Widget to update
  * @param value; Value to display, can be an empty array (ex: no value)
  *
  * @retval None
  */
void touchscreen_update(uint8_t *cmdCode,uint8_t *type,uint8_t *widgit, uint8_t* value, uint8_t valueArrSize, uint8_t toggleText){
	// Frame Header


		uint8_t cmdW[] = {0x63, 0x6D, 0x64, 0x5F, 0x63, 0x6F, 0x64, 0x65, 0x22};
		// 5C 22 63 6D 64 5F 63 6F 64 65 5C 22
		uint8_t typeW[] = {0x22, 0x74, 0x79, 0x70, 0x65, 0x22};
//		5C 22 74 79 70 65 5C 22
		uint8_t widgetW[] = {0x22, 0x77, 0x69, 0x64, 0x67, 0x65, 0x74,0x22};
//		5C 22 77 69 64 67 65 74 5C 22
		uint8_t textW[] = {0x22, 0x74, 0x65, 0x78, 0x74,0x22}; // "text"
		uint8_t valueW[] = {0x22, 0x76, 0x61, 0x6C, 0x75, 0x65, 0x22}; //"value"
//		5C 22 76 61 6C 75 65 5C 22

		// String Def
//		cmdW[] = " \"cmd_code\" ";
//		typeW[] = " \"type\" ";
//		widgetW[] = " \"widget\" ";
//		valueW[] = " \"value\" ";

		// Frame Header
		TxData[0] = 0x53;  // S
		TxData[1] = 0x54;  // T
		TxData[2] = 0x3C;  // <

		//Bracket
		TxData[3] = 0x7B; // {

		// CMD Code JSON
		int index = 4;
		uint8_t j = sizeof(cmdW)/sizeof(uint8_t);
		TxData[index] = 0x22; // "
		index++;
		for(int i = 0; i < j; i++)
		{
			TxData[index] = cmdW[i];
			index++;
		}
		TxData[index] =  0x3A; // :
		index++;

		// CMD Code Input
		int iterator = 0;
		TxData[index] = 0x22; // "
		index++;
		while(cmdCode[iterator] != 0x22)
		{
			TxData[index] = cmdCode[iterator];
			index++;
			iterator++;
		}
		TxData[index] = 0x22; // "
		index++;

		TxData[index] =  0x2C; // ,
		index++;


		// Type JSON
		j = sizeof(typeW)/sizeof(uint8_t);
		for(int i = 0; i < j; i++)
		{
			TxData[index] = typeW[i];
			index++;
		}
		TxData[index] =  0x3A; // :
		index++;

		// Type Input
		iterator = 0;
		TxData[index] = 0x22; // "
		index++;
		while(type[iterator] != 0x22)
		{
			TxData[index] = type[iterator];
			index++;
			iterator++;
		}
		TxData[index] = 0x22; // "
		index++;

		TxData[index] =  0x2C; // ,
		index++;

		// Widgit JSON
		j = sizeof(widgetW)/sizeof(uint8_t);
		for(int i = 0; i < j; i++)
		{
			TxData[index] = widgetW[i];
			index++;
		}
		TxData[index] =  0x3A; // :
		index++;

		// Widgit Input
		iterator = 0;
		TxData[index] = 0x22; // "
		index++;

		while(widgit[iterator] != 0x22)
		{
			TxData[index] = widgit[iterator];
			index++;
			iterator++;
		}
		TxData[index] = 0x22; // "
		index++;

		if(valueArrSize != 0)
		{
			TxData[index] =  0x2C; // ,
			index++;

			// Value JSON
			if(!toggleText)
			{
				j = sizeof(textW)/sizeof(uint8_t);
				for(int i = 0; i < j;i++)
				{
					TxData[index] =  textW[i];
					index++;
				}
				TxData[index] =  0x3A; // :
				index++;

				TxData[index] =  0x22; // "
				index++;
			} else {
				j = sizeof(valueW)/sizeof(uint8_t);
				for(int i = 0; i < j;i++)
				{
					TxData[index] =  valueW[i];
					index++;
				}
				TxData[index] =  0x3A; // :
				index++;
			}
//			TxData[index] = 0x2d; // -
//			index++;

			// Value input
			for(int i = 0; i < valueArrSize; i++)
			{
				TxData[index] = value[i];
				index++;
			}
			if(!toggleText)
			{
			TxData[index] = 0x64;  // d
			index++;
			TxData[index] = 0x42;  // B
			index++;
			TxData[index] = 0x22;  // "
			index++;
			}
		}
		// TERENCE NOTE!! MAYBE ADD ENDING QUOTE FOR NO VALUE INPUTS
		// HERE
//		else
//		{
//			TxData[index] = 0x22;  // "
//			index++;
//		}
		//Bracket
		TxData[index] = 0x7D;  // }
		index++;
		// Frame Tail
		TxData[index] = 0x3E; // >
		index++;
		TxData[index] = 0x45; // E
		index++;
		TxData[index] = 0x54; // T
		index++;
//		sizeof(TxData);

		HAL_UART_Transmit(&huart2,TxData, index,2000);
		clearTxData();
}

//VOA var between 1-12
void update_display_channel(uint8_t VOA, uint8_t* atten, uint8_t size)
{
	uint8_t cmdCode[] = {0x73, 0x65, 0x74,0x5F, 0x74, 0x65, 0x78, 0x74, 0x22}; // "set_text";
	uint8_t type[] = {0x6C, 0x61, 0x62, 0x65, 0x6C, 0x22};            //"label";
	uint8_t valArrSize = size;
	uint8_t temp, decimal_index;
	uint8_t value[12] = {0};
	uint8_t widgit[4] = {0x76, 0x6C, 0x31, 0x22};
	uint8_t widgitTens[5] = {0x76, 0x6C, 0x31, 0x30, 0x22};

	//"
		for(int i=0; i < valArrSize; i++){
			temp = atten[i];
			if(temp == 0x2E){
				value[i] = 0x2E;
				if(i == valArrSize-1) //adds 0 to end of array if decimal is pressed last
				{
					value[i+1] = 0x30;
					valArrSize++;
					break;
				}
			} else {
				value[i] = temp + 0x30;
			}
		}

	// Switch Statements to update widgit (VOA channnel)
	if(VOA == INPUT_FIELD_T)
	{
		widgit[0] = 0x69; // "in1"
		widgit[1] = 0x6E;
		widgit[2] = 0x31;
	}
	else if(VOA < 10)widgit[2] = 0x30 + VOA;
	else if(VOA == 10) widgitTens[3] = 0x30;   // "vl10";
	else if(VOA == 11) widgitTens[3] = 0x31;  // "vl11";
	else if(VOA == 12) widgitTens[3] = 0x32;   // "vl12";

	if(VOA < 10)touchscreen_update(cmdCode,type, widgit, value, valArrSize,0);
	else touchscreen_update(cmdCode,type, widgitTens, value, valArrSize,0);

}

void np_to_touchscreen(void){
	// Test
	uint8_t value[12] = {0};
	uint8_t valArrSize = np_buffer.index;
	uint8_t temp, decimal_index;
	for(int i=0; i<valArrSize; i++){
		temp = np_buffer.buff[i];
		if(temp == 0x2E){
			value[i] = 0x2E;
			if(i == valArrSize-1) //adds 0 to end of array if decimal is pressed last
			{
				value[i+1] = 0x30;
				valArrSize++;
				break;
			}
		} else {
			value[i] = temp + 0x30;
		}
	}
	uint8_t cmdCode[] = {0x73, 0x65, 0x74,0x5F, 0x74, 0x65, 0x78, 0x74, 0x22}; // "set_text";
	uint8_t type[] = {0x6C, 0x61, 0x62, 0x65, 0x6C, 0x22};            //"label";

	// Update Widgit

	uint8_t widgit[] = {0x76, 0x6C, 0x31, 0x22};        // "vl1";
	touchscreen_update(cmdCode,type, widgit, value, valArrSize,0);
}

void touchscreen_receive_callback(void){
	clearRxDataTemp();
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, RxData_temp, RX_DATA_TEMP_SIZE);
	touchscreenInputs = parseUART();
	parseTouchIn(touchscreenInputs);
	clearRxData();
	/* 1) Clear Rx_data
	 * 2) Parse touchscreenInputs
	 * 3) Set global Variables for CMD code ex..
	 */
}

void lockTouchscreen(void)
{
   uint8_t cmd[] = {0x6F, 0x70, 0x65, 0x6E, 0x5F, 0x77, 0x69, 0x6E, 0x22};
   uint8_t type[] = {0x77, 0x69, 0x6E, 0x64, 0x6F, 0x77, 0x22};
   uint8_t widgit[] = {0x70, 0x61, 0x75, 0x73, 0x65, 0x22}; // "pause"
   uint8_t temp[] = {};
   touchscreen_update(cmd,type,widgit,temp,0,0); // Lock TS
}

void selectChannel(void){
	uint8_t voaChannel;
	uint8_t selected = highlight;
	uint8_t cmdCode[] = {0x73, 0x65, 0x74,0x5F, 0x76, 0x61, 0x6C, 0x75, 0x65, 0x22}; //set_value
	uint8_t type[] = {0x70, 0x72, 0x6F, 0x67, 0x72, 0x65, 0x73, 0x73, 0x5F, 0x62, 0x61, 0x72, 0x22}; //progress_bar
	uint8_t widgit[3] = {0x73, 0x00, 0x22};
	uint8_t widgitTens[4] = {0x73, 0x31, 0x00, 0x22};
	uint8_t valueHundred[] = {0x31,0x30,0x30};
	uint8_t value[] = {0x30};

	// toggle touchscreen update to remove label setting
	uint8_t toggle = 1;
	// Channels 1 - 9

	for(int i=0; i<NUM_OF_VOAS; i++)
	{
		if(voaHighlighted & (1<<i))
		{
			voaChannel = i+1;
			voaHighlighted = voaHighlighted & ~(1<<i);
			break;
		}
	}

	if(voaChannel == 1) widgit[1] = 0x31;
	if(voaChannel == 2) widgit[1] = 0x32;
	if(voaChannel == 3) widgit[1] = 0x33;
	if(voaChannel == 4) widgit[1] = 0x34;
	if(voaChannel == 5) widgit[1] = 0x35;
	if(voaChannel == 6) widgit[1] = 0x36;
	if(voaChannel == 7) widgit[1] = 0x37;
	if(voaChannel == 8) widgit[1] = 0x38;
	if(voaChannel == 9) widgit[1] = 0x39;
	// Channels 10- 12
	if(voaChannel == 10) widgitTens[2] = 0x30;
	if(voaChannel == 11) widgitTens[2] = 0x31;
	if(voaChannel == 12) widgitTens[2] = 0x32;

	if(voaChannel < 10)
	{
		if(!selected) touchscreen_update(cmdCode,type,widgit,value,1,toggle);
		else touchscreen_update(cmdCode,type,widgit,valueHundred,3,toggle);
	}
	else
	{
		if(!selected) touchscreen_update(cmdCode,type,widgitTens,value,1,toggle);
		else touchscreen_update(cmdCode,type,widgitTens,valueHundred,3,toggle);
	}

	if(voaHighlighted != 0) add_scheduled_event(TOUCHSCREEN_SELECT_CB);
}
//
//void selectAllVOA(uint8_t highlight)
//{
//	for(int i=0; i<NUM_OF_VOAS; i++)
//	{
//		voaSelected = i+1;
//		highlight = highlight;
//		selectChannel();
//	}
//}
