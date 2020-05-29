/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "string.h"
/* USER CODE END 0 */

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_lpuart1_tx;
DMA_HandleTypeDef hdma_usart2_tx;

/* LPUART1 init function */

void MX_LPUART1_UART_Init(void)
{

  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 9600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspInit 0 */

  /* USER CODE END LPUART1_MspInit 0 */
    /* LPUART1 clock enable */
    __HAL_RCC_LPUART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**LPUART1 GPIO Configuration    
    PA0-CK_IN     ------> LPUART1_RX
    PA1     ------> LPUART1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_LPUART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* LPUART1 DMA Init */
    /* LPUART1_TX Init */
    hdma_lpuart1_tx.Instance = DMA1_Channel4;
    hdma_lpuart1_tx.Init.Request = DMA_REQUEST_5;
    hdma_lpuart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_lpuart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_lpuart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_lpuart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_lpuart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_lpuart1_tx.Init.Mode = DMA_NORMAL;
    hdma_lpuart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_lpuart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_lpuart1_tx);

    /* LPUART1 interrupt Init */
    HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(LPUART1_IRQn);
  /* USER CODE BEGIN LPUART1_MspInit 1 */

  /* USER CODE END LPUART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel2;
    hdma_usart2_tx.Init.Request = DMA_REQUEST_4;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspDeInit 0 */

  /* USER CODE END LPUART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPUART1_CLK_DISABLE();
  
    /**LPUART1 GPIO Configuration    
    PA0-CK_IN     ------> LPUART1_RX
    PA1     ------> LPUART1_TX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);

    /* LPUART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* LPUART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(LPUART1_IRQn);
  /* USER CODE BEGIN LPUART1_MspDeInit 1 */

  /* USER CODE END LPUART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

void rx_gps(uint8_t c);

uint8_t gps_just_locked = 0;

// UBX-CFG-TP5 message
#define GPSQRG 10000000


// GPS init String
// ATTENTION: if changed, the checksum must be recalculated
// use command line program: chksumcalc.c here in this folder
// 10 MHz
uint8_t gps_config_data[] = {
0xB5, 0x62, 0x06, 0x31, // Header UBX-CFG-TP5
0x20, 0x00,				// payload len = 32 Byte
0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
0x80, 0x96, 0x98, 0x00, // output freq if unlocked 10 MHz
0x80, 0x96, 0x98, 0x00,	// output freq if locked
0x00, 0x00, 0x00, 0x80,	// pule len ration if unlocked
0x00, 0x00, 0x00, 0x80, // pule len ration if locked
0x00, 0x00, 0x00, 0x00,	// unused user cfg
0xEF, 0x00, 0x00, 0x00, // flags
0xD5, 0x7A};			// CHKSUM

/* the RX Interrupt functionality of HAL is a big shit because it makes receiving a single byte impossible or complicated
 * instead the RX IRQ is enabled here
 */
void myUSARTrxInit()
{
	// set UART parameters
	UART_MASK_COMPUTATION(&hlpuart1);
	// enable UART1 and RX interrupt (no error interrupt, we dont need it)
	(&hlpuart1)->Instance->CR1=USART_CR1_RE|USART_CR1_TE|USART_CR1_UE|USART_CR1_RXNEIE;
}

#define FIFOLEN	10

uint8_t fifo1[FIFOLEN];
uint8_t wridx1=0, rdidx1=0;

int16_t read_fifo1()
{
int16_t data = -1;

	(&hlpuart1)->Instance->CR1 &= ~USART_CR1_RXNEIE;
	if(rdidx1 != wridx1)
	{
		data = fifo1[rdidx1];
		if(++rdidx1 == FIFOLEN) rdidx1=0;
	}
	(&hlpuart1)->Instance->CR1 |= USART_CR1_RXNEIE;
	return data;
}

void initGPS()
{
	HAL_UART_Transmit(&hlpuart1,gps_config_data,sizeof(gps_config_data),1000);
}

char fakeGGA[] = {"$GPGGA,233038.00,4849.28391,N,01307.74163,E,1,04,1.78,408.1,M,45.1,M,,*50\n\r"};
#define FAKE_BEGINTIME	2000
#define FAKE_ENDTIME	10000

int fakeDownconverter()
{
static int fake_cnt = 0;

	if(fake_cnt < 10)
	{
		// send 10 times after startup
		HAL_UART_Transmit(&huart2,(uint8_t *)fakeGGA,strlen(fakeGGA),1000);
		fake_cnt++;
		return 1;
	}

	return 0;
}

void sendUART()
{
int16_t data;

	// send messages from GPS module directly to the downconverter
	data = read_fifo1();
	if(data != -1)
	{
		uint8_t d = (data & 0xff);

		if(d == '$')
		{
			// send a fake GPS-Lock to the downconverter
			// this triggers an ICAL on its si5328 which is needed
			// to sync to the external 10 MHz provided by the GPS module
			if(fakeDownconverter() == 1)
				return;
		}

		// send to downconverter
		HAL_UART_Transmit(&huart2,&d,1,1000);

		// evaluate GPS message
		rx_gps(d);
	}
}

void myUART1rx(uint16_t data)
{
	// write into fifo1
	if(((wridx1+1)%FIFOLEN) == rdidx1)
		return; // fifo full

	fifo1[wridx1] = (uint8_t)data;
	if(++wridx1 == FIFOLEN) wridx1=0;
}

// receive from GPS module

volatile uint8_t ubx_ack = 0;
volatile uint8_t ubx_class = 0;
volatile uint8_t ubx_id = 0;
volatile uint16_t ubx_len = 0;
volatile uint16_t p_len = 0;

void rx_gps(uint8_t c)
{
static uint8_t stat = 0;
static uint16_t len_lsb;
static uint8_t commacnt=0;

	switch(stat)
	{
	case 0:	// wait for 0xb5 (UBX or $ (NMEA))
		if(c == 0xb5)
		{
			stat++;
		}
		if(c == '$')
		{
			stat = 100;
		}
		break;

	case 1:	// wait for 0x62
		if(c == 0x62)
		{
			stat++;
		}
		else
			stat = 0;	// invalid
		break;

	case 2: // get class
		ubx_class = c;
		if(c==1 || c==2 || c==4 || c==5 || c==6 || c==9 || c==0x0a || c==0x0b || c==0x0d || c==0x10 || c==0x13 || c==0x21 || c==0x27 || c==0x28)
			stat++;
		else
			stat = 0;	// invalid class
		break;

	case 3: // get ID
		ubx_id = c;
		stat ++;
		break;

	case 4: // get length LSB
		len_lsb = c;
		stat++;
		break;

	case 5: // get length MSB
		ubx_len = c;
		ubx_len <<= 8;
		ubx_len |= len_lsb;
		if(ubx_len == 0 || ubx_len > 85)
			stat = 0;	// finish, no payload
		else
		{
			stat++;
			p_len = 0;
		}
		break;

	case 6: // get payload
		if(ubx_class == 5)
		{
			ubx_ack = 1;
			// all read, finish
			stat = 0;
		}
		else
			stat = 0;
		break;

	// NMEA reception
	// $GPGGA,172814.0,3723.46587704,N,12202.26957864,W,2...
	// check the FIX indicator after the 6th comma
	case 100: if(c == 'G') stat++; else stat=0; break;
	case 101: if(c == 'P') stat++; else stat=0; break;
	case 102: if(c == 'G') stat++; else stat=0; break;
	case 103: if(c == 'G') stat++; else stat=0; break;
	case 104: if(c == 'A') {commacnt=0; stat++;} else stat=0; break;
	case 105:
		if(commacnt == 6)
		{
			if(c == '1' || c == '2')
			{
				HAL_GPIO_WritePin(GPIOC, LED1_Pin, GPIO_PIN_RESET);
				if(gps_just_locked == 0)
					gps_just_locked = 1;
			}
			else
			{
				HAL_GPIO_WritePin(GPIOC, LED1_Pin, GPIO_PIN_SET);
				gps_just_locked = 0;
			}
			stat = 0;
			break;
		}
		if(c == ',') commacnt++;
		if(commacnt > 6) stat = 0;
		break;
	}
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
