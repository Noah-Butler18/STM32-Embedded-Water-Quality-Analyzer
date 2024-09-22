/*
 * stm32f407_vg_tim_driver.c
 *
 *  Created on: Jul 2, 2024
 *      Author: butle
 */

#include "stm32f407vg_tim_driver.h"

uint32_t APB1;


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- TIM_PeriClockControl

 	 * @brief  		- enables or disables the clock of a TIM peripheral

 	 * @param 		- *pTIMx : TIM peripheral base address in MCU memory
 	 * @param  		- EnOrDi : macros to enable or disable the clock (ENABLE or DISABLE macros are in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void TIM_PeriClockControl(TIM2_5_RegDef_t *pTIMx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		if( (uint32_t) pTIMx == TIM2_BASE_ADDR )
		{
			TIM2_PCLK_EN();
		}
		else if( (uint32_t) pTIMx == TIM5_BASE_ADDR )
		{
			TIM5_PCLK_EN();
		}
	}
	else
	{
		if( (uint32_t) pTIMx == TIM2_BASE_ADDR )
		{
			TIM2_PCLK_DI();
		}
		else if( (uint32_t) pTIMx == TIM5_BASE_ADDR )
		{
			TIM5_PCLK_DI();
		}
	}
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- TIM2_5_SetDelayInit

 	 * @brief  		- This API uses the Internal peripheral APB1 clock to configure a timer with a desired frequency with respect to the
 	 * 				- APB1 peripheral clock

 	 * @param 		- *pTIMx : TIM peripheral base address in MCU memory

 	 * @retval 		- none

 	 * @Note		- Uses: Configure a timer peripheral
*/
void TIM2_5_SetDelayInit(TIM2_5_RegDef_t *pTIMx)
{
	//0. Turn on TIM peripheral
	TIM_PeriClockControl(pTIMx, ENABLE);

	//1. Enable update events
	pTIMx->CR1 &= ~( 1 << TIM2_5_CR1_UDIS );

	//2. Enable the setting of the UG bit to generate EVENTS (by disabling the URS bit)
	pTIMx->CR1 &= ~( 1 << TIM2_5_CR1_URS );

	//3. Enable pre-loading of ARR register
	pTIMx->CR1 &= ~( 1 << TIM2_5_CR1_ARPE );

	//Update global APB1 value every time TIM is initialized
	APB1 = RCC_GetPCLK1Val();
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- TIM2_5_GetFlagStatus

 	 * @brief  		- This API checks the status of any flag in the TIM2-5 SR register

 	 * @param 		- *pTIMx : TIM peripheral base address in MCU memory
 	 * @param  		- FlagName : Desired SR Flag to check for

 	 * @retval 		- data that is read from flag - can only be either 1 or 0 (set or reset)

 	 * @Note		- none
*/
uint8_t TIM2_5_GetFlagStatus(TIM2_5_RegDef_t *pTIMx, uint8_t FlagName)
{
	if(	pTIMx->SR & FlagName ) return FLAG_SET;
	else return FLAG_RESET;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- TIM2_5_SetInterrupt

 	 * @brief  		- This API uses the Internal peripheral APB1 clock to generate an interrupt with a desired frequency with respect to the
 	 * 				- APB1 peripheral clock

 	 * @param 		- *pTIMx : TIM peripheral base address in MCU memory
 	 * @param  		- FlagName : Desired SR Flag to check for

 	 * @retval 		- data that is read from flag - can only be either 1 or 0 (set or reset)

 	 * @Note		- none
*/
void TIM2_5_ClearFlag(TIM2_5_RegDef_t *pTIMx, uint8_t FlagName)
{
	pTIMx->SR &= ~( FlagName );
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- TIM2_5_Delay

 	 * @brief  		- This API uses the Internal peripheral APB1 clock to generate a timer with a desired frequency with respect to the
 	 * 				- APB1 peripheral clock

 	 * @param 		- *pTIMx : TIM peripheral base address in MCU memory
 	 * @param  		- MicroSeconds : Desired number of microseconds you want to elapse before TIM peripheral triggers

 	 * @retval 		- none

 	 * @Note		- Uses: blocking function that delays application for a specified amount of microseconds
*/
void TIM2_5_Delay(TIM2_5_RegDef_t *pTIMx, float MicroSeconds)
{
	//At 16*10^6Hz for TIM timer counter, that translates to 1 increment per 0.0625 microseconds
	//To translate user argument to TIM roll over register value, need to use simple proportion
	//Set auto-reload register value (this is the value at which the counter will generate an interrupt and automatically roll over)

	//1. Clear ARR and CNT registers
	pTIMx->ARR = RESET;
	pTIMx->CNT = RESET;

	double RollOverVal = ( ( ( (double) APB1 ) / 1000000 ) * MicroSeconds );

	//2. Write value into ARR register
	pTIMx->ARR = ( uint32_t ) RollOverVal;

	//3. Enable counter to begin counting
	pTIMx->CR1 |= ( 1 << TIM2_5_CR1_CEN );

	//4. Block processor until overflow (counter value reached)
	while( !TIM2_5_GetFlagStatus(pTIMx,TIM_FLAG_UIF) )
		;

	//5. Disable CNT from further incrementing so that it resets and stays at 0.
	pTIMx->CR1 &= ~( 1 << TIM2_5_CR1_CEN );

	//6. Clear update event (CNT = ARR) flag
	TIM2_5_ClearFlag(pTIMx,TIM_FLAG_UIF);
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- TIM2_5_SetInterrupt

 	 * @brief  		- This API uses the Internal peripheral APB1 clock to generate an interrupt with a desired frequency with respect to the
 	 * 				- APB1 peripheral clock

 	 * @param 		- *pTIMx : TIM peripheral base address in MCU memory
 	 * @param  		- Freq : Desired frequency (times per second) with which a user wants an interrupt generated

 	 * @retval 		- none

 	 * @Note		- Uses: generate an interrupt for the CPU at a known frequency/time interval
 	 * 				- This only applies to TIM2 to TIM5 peripherals

*/
void TIM2_5_SetIT(TIM2_5_RegDef_t *pTIMx, float freq)
{
	//0.5 Turn on TIM peripheral
	TIM_PeriClockControl(pTIMx, ENABLE);

	//1. Enable update events and enable update interrupts
	pTIMx->CR1 &= ~( 1 << TIM2_5_CR1_UDIS );

	pTIMx->DIER |= ( 1 << TIM2_5_DIER_UIE );

	//2. Enable the setting of the UG bit to generate interrupts (by disabling the URS bit)
	pTIMx->CR1 &= ~( 1 << TIM2_5_CR1_URS );

	//3. Disable pre-loading of ARR register (this doesn't matter unless needed for more precise timing with many changes of the ARR register)
	pTIMx->CR1 &= ~( 1 << TIM2_5_CR1_ARPE );

	//4. Set auto-reload register value (this is the value at which the counter will generate an interrupt and automatically roll over)

	//4.1 Calculate ARR value
	uint32_t APB1 = RCC_GetPCLK1Val();
	double RollOverVal = ( ( (double) APB1 ) / freq );		//This has the effect of speeding up or slowing down the rate at which interrupts are generated by the counter overflowing


	//4.2 Check to make sure uint32_t value is valid
	if( RollOverVal > MAX_UINT32_VAL )
	{
		while(1);	//hang-up program if user requested an invalid number
		//For APB1 = 16MHz, Freq should not be smaller than 0.00372529
	}


	//4.3 Write ARR value
	uint32_t temp = RollOverVal;
	pTIMx->ARR = temp;

	//5. Enable the counter to begin counting
	pTIMx->CR1 |= ( 1 << TIM2_5_CR1_CEN );

	//x. Set UG bit once to latch changes to registers?
}



/*********************** Function Documentation ***************************************
 *
 	 * @fn			- TIM2_5_IRQInterruptConfig

 	 * @brief  		- API that configures interrupts generated by a TIM peripheral. This function handles the configurations needed for interrupts on the processor side
 	 * 				- Refer to processor guide here for more details: https://www.engr.scu.edu/~dlewis/book3/docs/Cortex-M4_Devices_Generic_User_Guide.pdf

 	 * @param 		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table
 	 * @param 		- EnOrDi : macros to enable or disable the IRQ (ENABLE or DISABLE macros in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void TIM2_5_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//configure ISER0 register in processor //0 to 31
			*NVIC_ISER0 |= ( 1 << IRQNumber );
		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			//configure ISER1 register in processor //32 to 63
			*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			//configure ISER2 register in processor //64 to 95
			*NVIC_ISER2 |= ( 1 << (IRQNumber % 32) );
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			//configure ICER0 register in processor //0 to 31
			*NVIC_ICER0 |= ( 1 << IRQNumber );
		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			//configure ICER1 register in processor //32 to 63
			*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			//configure ICER2 register in processor //64 to 95
			*NVIC_ICER2 |= ( 1 << (IRQNumber % 32) );
		}
	}
}





/*********************** Function Documentation ***************************************
 *
	 * @fn			- TIM2_5_IRQPriorityConfig

	 * @brief  		- API that configures the priority level of a given IRQ (TIM-driven interrupt).

	 * @param 		- IRQPriority : Value that contains priority level of interrupt as compared to other interrupts. This is handled on the processor side in Cortex-M4 internal peripheral registers
	 * @param		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table

	 * @retval 		- none

	 * @Note		- none

*/
void TIM2_5_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. Find out which IPR register the IRQ is in
	uint8_t Offset = IRQNumber / 4;

	//2. Clear and write by shifting the priority value into the correct position
	uint8_t temp = (IRQNumber % 4) * 8;

	uint8_t shiftAmount = temp + ( 8 - NO_PR_BITS_IMPLEMENTED );														//Note: In the Cortex-M4, only 4 most significant bits of each of the 4 "sections" in the Interrupt priority register (IPR) are implemented, not the last 4. Because if this, you are limited to only 16 priority levels instead of 256 (this was for cost savings apparently). Keeping this in mind, you must also shift whatever priority level desired over to the left by 4 bits no matter the section of the byte it is in.
	*(NVIC_IPR_BASE_ADDR + (Offset) ) &= ~( 0xFF << shiftAmount );														//Clear the existing priority level
	*(NVIC_IPR_BASE_ADDR + (Offset) ) |= ( IRQPriority << shiftAmount );												//Write the desired priority level

}


/*********************** Function Documentation ***************************************
 *
	 * @fn			- TIM2_5_IRQPriorityConfig

	 * @brief  		- API that configures the priority level of a given IRQ (TIM-driven interrupt).

	 * @param 		- IRQPriority : Value that contains priority level of interrupt as compared to other interrupts. This is handled on the processor side in Cortex-M4 internal peripheral registers
	 * @param		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table

	 * @retval 		- none

	 * @Note		- none

*/
void TIM2_5_IRQHandling(TIM2_5_RegDef_t *pTIMx)
{
	// Reset UIF flag in SR for after an update event occurs, otherwise interrupts will continue to happen over and over again
	TIM2_5_ClearFlag(pTIMx, TIM_FLAG_UIF);
}

