/**
  ******************************************************************************
  * @file    Template_2/stm32f0_uart.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   UART functions.
  ******************************************************************************
  * @attention
  *
  * Use this functions to configure serial comunication interface (UART).
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "main.h"

#include <string.h>




//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//

//#define USE_USARTx_TIMEOUT

#ifdef USE_USARTx_TIMEOUT
#define BUFFRX_TIMEOUT 200 //200ms
#define BUFFTX_TIMEOUT 200 //200ms
#endif


//--- VARIABLES EXTERNAS ---//
//extern volatile unsigned char buffrx_ready;
//extern volatile unsigned char *pbuffrx;

extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned char dmx_receive_flag;
extern volatile unsigned char DMX_channel_received;
extern volatile unsigned char DMX_channel_selected;
extern volatile unsigned char DMX_channel_quantity;
extern volatile unsigned char data1[];
//static unsigned char data_back[10];
extern volatile unsigned char data[];


//--- Private variables ---//
//Reception buffer.

//Transmission buffer.

//--- Private function prototypes ---//
//--- Private functions ---//

void Usart_Time_1ms (void)
{
#ifdef USE_USARTx_TIMEOUT
	if (buffrx_timeout > 1)
		buffrx_timeout--; //Se detiene con buffrx_timeout = 1.

	if (bufftx_timeout > 1)
		bufftx_timeout--; //Se detiene con bufftx_timeout = 1.
#endif
}
//-------------------------------------------//
// @brief  UART configure.
// @param  None
// @retval None
//------------------------------------------//
void USARTx_IRQHandler(void)
{
	unsigned short i;
	unsigned char dummy;

	/* USART in mode Receiver --------------------------------------------------*/
	if (USART_GetITStatus(USARTx, USART_IT_RXNE) == SET)
	{
		//RX DMX
		//data0 = USART_ReceiveData(USART3);

		if (dmx_receive_flag)
		{
			if (DMX_channel_received == 0)		//empieza paquete
				LED_ON;

			data1[DMX_channel_received] = USARTx->RDR & 0x0FF;
			if (DMX_channel_received < 511)
				DMX_channel_received++;
			else
				DMX_channel_received = 0;

#ifdef WITH_GRANDMASTER
			if (DMX_channel_received >= (DMX_channel_selected + DMX_channel_quantity + 1))
			{
				//los paquetes empiezan en 0 pero no lo verifico
				for (i=0; i < (DMX_channel_quantity + 1); i++)
				{
					data[i] = data1[(DMX_channel_selected) + i];
					//data[4] = 0;
				}

				//--- Reception end ---//
				DMX_channel_received = 0;
				//USARTx_RX_DISA;
				dmx_receive_flag = 0;
				Packet_Detected_Flag = 1;
				LED_OFF;	//termina paquete
			}
#else
			if (DMX_channel_received >= (DMX_channel_selected + DMX_channel_quantity))
			{
				//los paquetes empiezan en 0 pero no lo verifico
				for (i=0; i<DMX_channel_quantity; i++)
				{
					data[i] = data1[(DMX_channel_selected) + i];
				}

				/*
				if ((data[0] < 10) || (data[0] > 240))	//revisa el error de salto de canal
					LED2_ON;
				else
					LED2_OFF;	//trata de encontrar el error de deteccion de trama
				*/

				//--- Reception end ---//
				DMX_channel_received = 0;
				//USARTx_RX_DISA;
				dmx_receive_flag = 0;
				Packet_Detected_Flag = 1;
				LED_OFF;	//termina paquete
			}
#endif
		}
		else
			USARTx->RQR |= 0x08;	//hace un flush de los datos sin leerlos
	}

	/* USART in mode Transmitter -------------------------------------------------*/
	if (USART_GetITStatus(USARTx, USART_IT_TXE) == SET)
	{
		USARTx->CR1 &= ~0x00000088;	//bajo TXEIE bajo TE
	}


	if ((USART_GetITStatus(USARTx, USART_IT_ORE) == SET) ||
			(USART_GetITStatus(USARTx, USART_IT_NE) == SET) ||
			(USART_GetITStatus(USARTx, USART_IT_FE) == SET))
	{
		USARTx->ICR |= 0x0e;
		dummy = USARTx->RDR;
		return;
	}
}

void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);

  /* Enable USART clock */
  USARTx_APBPERIPHCLOCK(USARTx_CLK, ENABLE);

  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);

  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
  GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
  GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follow:
  - BaudRate = 9600 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  //USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_BaudRate = 250000;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);

  /* NVIC configuration */
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


#ifdef USE_USARTx_TIMEOUT
  buffrx_timeout = 0;
  bufftx_timeout = 0;
#endif

  /* Enable USART */
  USART_Cmd(USARTx, ENABLE);

  //--- Enable receiver interrupt ---//
  USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
}

void USARTSend(unsigned char value)
{
	while ((USARTx->ISR & 0x00000080) == 0);
	USARTx->TDR = value;
}

//--- end of file ---//

