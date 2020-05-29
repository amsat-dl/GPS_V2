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
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */
#define I2C_ADDR_SI5328  0xd0 //(0x68<<1)

void delay_1ms(uint32_t del);
void si5328_ical();

extern uint8_t jumpers;
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00707CBB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**I2C1 GPIO Configuration    
    PA9     ------> I2C1_SCL
    PA10     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* I2C1 interrupt Init */
    HAL_NVIC_SetPriority(I2C1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_IRQn);
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PA9     ------> I2C1_SCL
    PA10     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* I2C1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C1_IRQn);
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

typedef struct {
    uint8_t addr;
    uint8_t value;
} reg_set;

#define _LSB(x)  ((x-1) & 0xff)
#define _MSB(x)  (((x-1) >> 8))
#define _HIGHNIB(x) ((x-4)<<5)

#define FREE_RUN	0x40	// enable free run mode -> XAXB connected to CKIN2
#define CK_PRIOR2	0x04	// CKIN1 (GPS) has priority
#define CK_PRIOR1	0x00
#define BWSEL_REG	0xf0	// High BW: lower input jitter attenuation, but less self generated jitter
							// Low BW (higher reg val): better input jitter attenuation, but more self generated jitter
							// GPS module: higher values work much better
							// Bodnar always uses 15
#define CKSEL_CKIN2	0x40	// use CKIN2 fot XA/XB connection (free run mode))
#define AUTOSEL_REG	0x80	// auto revertive selection
#define HIST_DEL	0x17	// 10111 = 839 ms (longer than default for si5328 according to ref manual)
#define HIST_AVG	0xC8	// 11001 = 13,422 history averaging time
#define SFOUT2_REG	0x10	// CKOUT2: CMOS levels
#define SFOUT1_REG	0x05	// CKOUT1: LVPECL levels
#define FOS_THR		0x20	// we don't use FOS, so this setting has no meaning
#define VALTIME		0x08	// alarm remove time 100ms
#define LOCKT		0x01	// Retrigger intv. 106ms, this long time give better sync (ref. man recommends min. 53ms)
							// the LOCKT setting has no influence on jitter

// register set without clock divider settings
static const reg_set reg_set_univ_CKIN1_GPS[] = {

	{ 0, 0x14 },						// no free run
	{ 1, 0xe0 | CK_PRIOR2 | CK_PRIOR1},
	{ 2, 0x02 | BWSEL_REG},
	{ 3, 0x05 },						// use CKIN_1
	{ 4, 0x00 | HIST_DEL},				// manually select CKIN_1
    { 6, 0x00 | SFOUT2_REG | SFOUT1_REG},
	{ 9, 0x00 | HIST_AVG},
	{19, 0x00 | FOS_THR | VALTIME | LOCKT},
	{20, 0x3e},		// activate LOL output and INT_C1B output
    {21, 0xfc},		// ignore CSEL Pin -> Register 3 selects clock
	{55, 0x08 },
	{137, 0x01},	// fast lock

	// enable monitoring
	// LOS monitoring in Reg. 138 is enabled by default
	// LOS and FOS monitoring in Reg. 139 is enabled by default
	// disable FOS because we do not have any reference signal
	{139, 0xfc},
};

// register set for various clocks
// OutClk2 is always 10 MHz and is used as ext. reference of the downconverter
// OutClk1 has various frequencies depending on the jumper settings

// OutClk1 = 20 MHz
static const reg_set reg_set_clk_20MHZ[10] = {
	// N31: divides Clk1 input to the DSPLL clock
	{44 ,_MSB(5)},
	{45 ,_LSB(5)},

	// N2: DSPLL divider (multiplier)
	{40 ,_HIGHNIB(5)},
	{41 ,_MSB(560)},
	{42, _LSB(560)},

	// N1HS: DPLL output divider, first high speed stage
	{25, _HIGHNIB(7)},

	// NC1/2LS: divides output of N1HS to get the CLKOUT1 and CLKOUT2 clocks
	{32, _MSB(40)},
	{33, _LSB(40)},
	{35, _MSB(80)},
	{36, _LSB(80)},
};

// OutClk1 = 25 MHz
static const reg_set reg_set_clk_25MHZ[10] = {
	// N31: divides Clk1 input to the DSPLL clock
	{44 ,_MSB(5)},
	{45 ,_LSB(5)},

	// N2: DSPLL divider (multiplier)
	{40 ,_HIGHNIB(5)},
	{41 ,_MSB(560)},
	{42, _LSB(560)},

	// N1HS: DPLL output divider, first high speed stage
	{25, _HIGHNIB(7)},

	// NC1/2LS: divides output of N1HS to get the CLKOUT1 and CLKOUT2 clocks
	{32, _MSB(32)},
	{33, _LSB(32)},
	{35, _MSB(80)},
	{36, _LSB(80)},
};

// OutClk1 = 27 MHz
static const reg_set reg_set_clk_27MHZ[10] = {
	// N31: divides Clk1 input to the DSPLL clock
	{44 ,_MSB(5)},
	{45 ,_LSB(5)},

	// N2: DSPLL divider (multiplier)
	{40 ,_HIGHNIB(9)},
	{41 ,_MSB(300)},
	{42, _LSB(300)},

	// N1HS: DPLL output divider, first high speed stage
	{25, _HIGHNIB(10)},

	// NC1/2LS: divides output of N1HS to get the CLKOUT1 and CLKOUT2 clocks
	{32, _MSB(20)},
	{33, _LSB(20)},
	{35, _MSB(54)},
	{36, _LSB(54)},
};

// OutClk1 = 30 MHz
static const reg_set reg_set_clk_30MHZ[10] = {
	// N31: divides Clk1 input to the DSPLL clock
	{44 ,_MSB(5)},
	{45 ,_LSB(5)},

	// N2: DSPLL divider (multiplier)
	{40 ,_HIGHNIB(5)},
	{41 ,_MSB(528)},
	{42, _LSB(528)},

	// N1HS: DPLL output divider, first high speed stage
	{25, _HIGHNIB(8)},

	// NC1/2LS: divides output of N1HS to get the CLKOUT1 and CLKOUT2 clocks
	{32, _MSB(22)},
	{33, _LSB(22)},
	{35, _MSB(66)},
	{36, _LSB(66)},
};

// OutClk1 = 40 MHz
static const reg_set reg_set_clk_40MHZ[10] = {
	// N31: divides Clk1 input to the DSPLL clock
	{44 ,_MSB(5)},
	{45 ,_LSB(5)},

	// N2: DSPLL divider (multiplier)
	{40 ,_HIGHNIB(5)},
	{41 ,_MSB(560)},
	{42, _LSB(560)},

	// N1HS: DPLL output divider, first high speed stage
	{25, _HIGHNIB(7)},

	// NC1/2LS: divides output of N1HS to get the CLKOUT1 and CLKOUT2 clocks
	{32, _MSB(20)},
	{33, _LSB(20)},
	{35, _MSB(80)},
	{36, _LSB(80)},
};

// OutClk1 = 50 MHz
static const reg_set reg_set_clk_50MHZ[10] = {
	// N31: divides Clk1 input to the DSPLL clock
	{44 ,_MSB(5)},
	{45 ,_LSB(5)},

	// N2: DSPLL divider (multiplier)
	{40 ,_HIGHNIB(5)},
	{41 ,_MSB(560)},
	{42, _LSB(560)},

	// N1HS: DPLL output divider, first high speed stage
	{25, _HIGHNIB(7)},

	// NC1/2LS: divides output of N1HS to get the CLKOUT1 and CLKOUT2 clocks
	{32, _MSB(16)},
	{33, _LSB(16)},
	{35, _MSB(80)},
	{36, _LSB(80)},
};

// OutClk1 = 100 MHz
static const reg_set reg_set_clk_100MHZ[10] = {
	// N31: divides Clk1 input to the DSPLL clock
	{44 ,_MSB(5)},
	{45 ,_LSB(5)},

	// N2: DSPLL divider (multiplier)
	{40 ,_HIGHNIB(5)},
	{41 ,_MSB(560)},
	{42, _LSB(560)},

	// N1HS: DPLL output divider, first high speed stage
	{25, _HIGHNIB(7)},

	// NC1/2LS: divides output of N1HS to get the CLKOUT1 and CLKOUT2 clocks
	{32, _MSB(8)},
	{33, _LSB(8)},
	{35, _MSB(80)},
	{36, _LSB(80)},
};

// OutClk1 = 100 MHz OutClk2 = 40 MHz
static const reg_set reg_set_clk_40_100MHZ[10] = {
	// N31: divides Clk1 input to the DSPLL clock
	{44 ,_MSB(5)},
	{45 ,_LSB(5)},

	// N2: DSPLL divider (multiplier)
	{40 ,_HIGHNIB(5)},
	{41 ,_MSB(560)},
	{42, _LSB(560)},

	// N1HS: DPLL output divider, first high speed stage
	{25, _HIGHNIB(7)},

	// NC1/2LS: divides output of N1HS to get the CLKOUT1 and CLKOUT2 clocks
	{32, _MSB(8)},
	{33, _LSB(8)},
	{35, _MSB(20)},
	{36, _LSB(20)},
};

uint8_t *dividers[] = {
	(uint8_t *)(&reg_set_clk_20MHZ[0]),
	(uint8_t *)(&reg_set_clk_25MHZ[0]),
	(uint8_t *)(&reg_set_clk_27MHZ[0]),
	(uint8_t *)(&reg_set_clk_30MHZ[0]),
	(uint8_t *)(&reg_set_clk_40MHZ[0]),
	(uint8_t *)(&reg_set_clk_50MHZ[0]),
	(uint8_t *)(&reg_set_clk_100MHZ[0]),
	(uint8_t *)(&reg_set_clk_40_100MHZ[0]),
};

void Si5328Ready()
{
	while(1)
	{
		HAL_StatusTypeDef r = HAL_I2C_IsDeviceReady(&hi2c1,I2C_ADDR_SI5328,10,1000);
		if(r != HAL_OK)
		{
			// LED1 blink: Si5328 not found: Hardware Error!
			HAL_GPIO_TogglePin(GPIOC, LED1_Pin);
			delay_1ms(100);
		}
		else
		{
			// Si5328 found, send INIT strings

			uint8_t resetcmd[2] = {136,0x80};
			HAL_I2C_Master_Transmit(&hi2c1,I2C_ADDR_SI5328,resetcmd,2,1000);
			delay_1ms(100);

			// set standard settings
			uint8_t *p = (uint8_t *)(&reg_set_univ_CKIN1_GPS[0]);
			for(int i=0; i<sizeof(reg_set_univ_CKIN1_GPS); i+=2)
			{
				uint8_t data[2];
				data[0] = *p++;
				data[1] = *p++;
				HAL_I2C_Master_Transmit(&hi2c1,I2C_ADDR_SI5328,data,2,1000);
				delay_1ms(1);
			}

			// set dividers, for SMA-2 connector, depending on jumper settings
			p = dividers[jumpers];
			for(int i=0; i<sizeof(reg_set_clk_100MHZ); i+=2)
			{
				uint8_t data[2];
				data[0] = *p++;
				data[1] = *p++;
				HAL_I2C_Master_Transmit(&hi2c1,I2C_ADDR_SI5328,data,2,1000);
				delay_1ms(1);
			}

			// do ICAL
			si5328_ical();

			break;
		}
	}
}

void si5328_ical()
{
	uint8_t icalcmd[2] = {136,0x40};
	HAL_I2C_Master_Transmit(&hi2c1,I2C_ADDR_SI5328,icalcmd,2,1000);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
