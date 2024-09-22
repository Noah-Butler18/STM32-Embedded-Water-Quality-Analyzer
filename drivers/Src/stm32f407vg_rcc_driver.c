/*
 * stm32f407vg_rcc_driver.c
 *
 *  Created on: Apr 17, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"



/*********************** Function Documentation ***************************************
 *
 	 * @fn			- RCC_GetPCLK1Val

 	 * @brief  		- Returns the value (in Hz) of the frequency that the APB1 bus is currently operating at

 	 * @param 		- none
 	 *
 	 * @retval 		- pclk1 : Value in Hz of the operating frequency of the APB1 bus

 	 * @Note		- none

*/
uint32_t RCC_GetPCLK1Val(void)
{
	uint32_t SystemClk, pclk1;
	uint8_t clksrc = (( RCC->CFGR >> 2 ) & 0x3);

	//determine system clock speed

	if( clksrc == 0 ) //HSI oscillator used as the system clock
	{
		SystemClk = 16000000;
	}
	else if( clksrc == 1 ) //HSE oscillator used as the system clock
	{
		SystemClk = 8000000;
	}
	else if( clksrc == 2 ) //PLL oscillator used as the system clock
	{
		//SystemClk = RCC_GetPLLOutputClk();
	}
	else
		SystemClk = 0;

	//Calculate APB1 speed based off of system clock speed

	//1. Get AHB Prescaler value
	uint16_t AHBPrescaler = 1;
	uint8_t temp = (( RCC->CFGR >> 4 ) & 0xF);

	if( temp >= 8 )
	{
		AHBPrescaler = 2;
		for( uint8_t i = 9; i <= temp; i++ ) //refer to bits 7:4 in RCC_CFGR for why this exists
		{
			AHBPrescaler *= 2;
		}
	}

	//2. Get the  APB1 prescaler
	uint8_t APB1Prescaler = 1;
	temp = (( RCC->CFGR >> 10 ) & 0x7);

	if( temp >= 4 )
	{
		APB1Prescaler = 2;
		for( uint8_t i = 5; i <= temp; i++ ) //refer to bits 12:10 in RCC_CFGR for why this exists
		{
			APB1Prescaler *= 2;
		}
	}


	//Have everything needed for calculating APB1 bus clock speed that APB2 peripherals are hanging off of

	pclk1 = ( (SystemClk / AHBPrescaler) / APB1Prescaler );

	return pclk1;
}



/*********************** Function Documentation ***************************************
 *
 	 * @fn			- RCC_GetPCLK2Val

 	 * @brief  		- Returns the value (in Hz) of the frequency that the APB2 bus is currently operating at

 	 * @param 		- none
 	 *
 	 * @retval 		- pclk1 : Value in Hz of the operating frequency of the APB2 bus

 	 * @Note		- none

*/
uint32_t RCC_GetPCLK2Val(void)
{
	uint32_t SystemClk, pclk2;
	uint8_t clksrc = (( RCC->CFGR >> 2 ) & 0x3);

	//determine system clock speed

	if( clksrc == 0 ) //HSI oscillator used as the system clock
	{
		SystemClk = 16000000;
	}
	else if( clksrc == 1 ) //HSE oscillator used as the system clock
	{
		SystemClk = 8000000;
	}
	else if( clksrc == 2 ) //PLL oscillator used as the system clock
	{
		//SystemClk = RCC_GetPLLOutputClk();
	}
	else
		SystemClk = 0;

	//Calculate APB2 speed based off of system clock speed

	//1. Get AHB Prescaler value
	uint16_t AHBPrescaler = 1;
	uint8_t temp = (( RCC->CFGR >> 4 ) & 0xF);

	if( temp >= 8 )
	{
		AHBPrescaler = 2;
		for( uint8_t i = 9; i <= temp; i++ ) //refer to bits 7:4 in RCC_CFGR for why this exists
		{
			AHBPrescaler *= 2;
		}
	}

	//2. Get the  APB2 prescaler
	uint8_t APB2Prescaler = 1;
	temp = (( RCC->CFGR >> 13 ) & 0x7);

	if( temp >= 4 )
	{
		APB2Prescaler = 2;
		for( uint8_t i = 5; i <= temp; i++ ) //refer to bits 12:10 in RCC_CFGR for why this exists
		{
			APB2Prescaler *= 2;
		}
	}


	//Have everything needed for calculating APB2 bus clock speed that APB2 peripherals are hanging off of

	pclk2 = ( (SystemClk / AHBPrescaler) / APB2Prescaler );

	return pclk2;
}

