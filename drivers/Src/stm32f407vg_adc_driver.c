/*
 * stm32f407vg_adc_driver.c
 *
 *  Created on: May 17, 2024
 *      Author: butle
 */

#include "stm32f407vg_adc_driver.h"

/*********** Driver-specific helper functions prototype section ***********/
static void ADC_SampleTimeInit(ADC_Handle_t *pADCHandle);
static void ADC_tStabDelay(void);
static void ADC_HandleRead(ADC_Handle_t *pADCHandle);

/********************************************************/




/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_PeriClockControl

 	 * @brief  		- enables or disables the clock of a ADC peripheral

 	 * @param 		- *pADCx : ADC peripheral base address in MCU memory
 	 * @param  		- EnOrDi : macros to enable or disable the clock (ENABLE or DISABLE macros are in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_PeriClockControl(ADC_RegDef_t *pADCx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		if( (uint32_t) pADCx == ADC1_BASE_ADDR )
		{
			ADC1_PCLK_EN();
		}
		else if ( (uint32_t) pADCx == ADC2_BASE_ADDR )
		{
			ADC2_PCLK_EN();
		}
		else if ( (uint32_t) pADCx == ADC3_BASE_ADDR )
		{
			ADC3_PCLK_EN();
		}
		else
			;
	}
	else
	{
		if( (uint32_t) pADCx == ADC1_BASE_ADDR )
		{
			ADC1_PCLK_DI();
		}
		else if ( (uint32_t) pADCx == ADC2_BASE_ADDR )
		{
			ADC2_PCLK_DI();
		}
		else if ( (uint32_t) pADCx == ADC3_BASE_ADDR )
		{
			ADC3_PCLK_DI();
		}
		else
			;
	}
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_PeripheralOnOffControl

 	 * @brief  		- API that turns on or off the ADCx peripheral

 	 * @param 		- *pADCx : ADC peripheral base address in MCU memory
 	 * @param 		- EnOrDi : User-inputted value to enable or disable the device's ADC peripheral

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_PeripheralOnOffControl(ADC_RegDef_t *pADCx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		pADCx->CR2 |= (1 << ADC_CR2_ADON);

		//Wait tSTAB time for ADC to stabilize after being turned on (approx 3us - see data sheet table 67)
		ADC_tStabDelay();
	}
	else if( EnOrDi == DISABLE )
	{
		pADCx->CR2 &= ~(1 << ADC_CR2_ADON);
	}
	else
		;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_GetFlagStatus

 	 * @brief  		- API that returns that value of a specific flag in the ADC peripheral status registers

 	 * @param 		- *pADCx : ADC peripheral base address in MCU memory
 	 * @param 		- FlagName : Name of the desired flag you want to check the status of

 	 * @retval 		- data that is read from flag - can only be either 1 or 0 (set or reset)

 	 * @Note		- none

*/
uint8_t ADC_GetFlagStatus(ADC_RegDef_t *pADCx, uint32_t FlagName)
{
	if(	pADCx->SR & FlagName ) return FLAG_SET;
	else return FLAG_RESET;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_ClearFlag

 	 * @brief  		- Helper API that performs a software clear of an ADC status flag

 	 * @param 		- *pADCx : ADC peripheral base address in MCU memory
 	 * @param 		- FlagName : Name of the desired flag you want to clear

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_ClearFlag(ADC_RegDef_t *pADCx, uint32_t FlagName)
{
	switch( FlagName )
	{
		case ADC_FLAG_AWD:
			pADCx->SR &= ~( 1 << ADC_SR_AWD );
			break;

		case ADC_FLAG_JEOC:
			pADCx->SR &= ~( 1 << ADC_SR_JEOC );
			break;

		case ADC_FLAG_JSTRT:
			pADCx->SR &= ~( 1 << ADC_SR_JSTRT );
			break;

		case ADC_FLAG_STRT:
			pADCx->SR &= ~( 1 << ADC_SR_STRT );
			break;

		case ADC_FLAG_OVR:
			pADCx->SR &= ~( 1 << ADC_SR_OVR );
			break;

		default:
			break;
	}
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_Init

 	 * @brief  		- API that initializes a specific ADC peripheral as well as the common ADC registers with
 	 * 				- programmer-defined values from the ADC_Config_t structure

 	 * @param 		- *pADCHandle : contains ADC peripheral base address in MCU memory as well as configuration values

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_Init(ADC_Handle_t *pADCHandle)
{
	// 0. Turn on peripheral clock
	ADC_PeriClockControl(pADCHandle->pADCx, ENABLE);

	//0.5 Turn on ADC
	ADC_PeripheralOnOffControl(pADCHandle->pADCx, ENABLE);


	uint32_t tempreg = 0;

	// 1. Configure clock APB2 clock prescaler for ADCCLK
	tempreg |= ( pADCHandle->ADC_Config.ADC_ClkPrescaler << ADC_CCR_ADCPRE_1_0 );
	ADCCOMMON->CCR |= tempreg;

	// 2. Configure ADC data register resolution
	tempreg = 0;
	tempreg |= ( pADCHandle->ADC_Config.ADC_Resolution << ADC_CR1_RES_1_0 );
	pADCHandle->pADCx->CR1 |= tempreg;

	// 3. Configure data register bit alignment
	tempreg = 0;
	tempreg |= ( pADCHandle->ADC_Config.ADC_DataAlignment << ADC_CR2_ALIGN );
	pADCHandle->pADCx->CR2 |= tempreg;

	// 4. Configure ADC mode

	if( pADCHandle->ADC_Config.ADC_Mode == ADC_SINGLE_CONVERSION_MODE )
	{
		//a. Single conversion mode

		//4.1a Set CONT bit to 0
		pADCHandle->pADCx->CR2 &= ~(1 << ADC_CR2_CONT);
	}
	else if( pADCHandle->ADC_Config.ADC_Mode == ADC_CONT_CONVERSION_MODE )
	{
		//b. Continuous conversion mode

		//4.1b Set CONT bit to 1
		pADCHandle->pADCx->CR2 |= (1 << ADC_CR2_CONT);
	}
	else if( pADCHandle->ADC_Config.ADC_Mode == ADC_SCAN_CONVERSION_MODE )
	{
		//c. Scan conversion mode

		//4.1c Set CONT bit to 0 IF YOU ONLY WANT 1 CONVERSION PER IRQ TRIGGER
		pADCHandle->pADCx->CR2 &= ~(1 << ADC_CR2_CONT);

		//4.2c Set scan bit = 1
		pADCHandle->pADCx->CR1 |= (1 << ADC_CR1_SCAN );
	}
	else if( pADCHandle->ADC_Config.ADC_Mode == ADC_DISCONT_CONVERSION_MODE )
	{
		//d. Discontinuous conversion mode
	}

	// 5. Configure ADC sequence length and order

		//a. Configure sequence conversion length
		tempreg = 0;
		tempreg = ( pADCHandle->ADC_Config.ADC_Seq_Len << ADC_SQR1_L_3_0 );

		if( ( pADCHandle->ADC_Config.ADC_Seq_Len ) > 15)
		{
			//Cannot have a sequence of regular ADC conversions longer than 16 - See RM 13.3.3. If invalid number, enter into an infinite loop.
			while(1);
		}

		pADCHandle->pADCx->SQR1 |= tempreg;

		//b. Configure channel conversion sequence

		ADC_SequenceInit(pADCHandle);


	// 6. Configure ADC sampling time
	ADC_SampleTimeInit(pADCHandle);

	// 7. Enable and configure analog watch dog higher and lower threshold values (interrupts enabled in ADC_ConvertIT API)
	pADCHandle->pADCx->CR1 |= ( 1 << ADC_CR1_AWDEN );

	tempreg = 0;
	tempreg = pADCHandle->ADC_Config.ADC_AWDHT;
	pADCHandle->pADCx->HTR = tempreg;

	tempreg = 0;
	tempreg = pADCHandle->ADC_Config.ADC_AWDLT;
	pADCHandle->pADCx->LTR |= tempreg;


	//8. Enable overrun detection (interrupts enabled in ADC_ConvertIT API)
	//This also configures the EOC status bit to be set after each conversion and not just at the end of the sequence
	pADCHandle->pADCx->CR2 |= ( 1 << ADC_CR2_EOCS );
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_DeInit

 	 * @brief  		- API that writes reset values into all of an ADC peripheral's registers

 	 * @param 		- none

 	 * @retval 		- none

 	 * @Note		- ADC interface reset is common to all ADCs - all ADC's peripheral registers will be reset

*/
void ADC_DeInit(void)
{
	ADC_REG_RESET();
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_Init

 	 * @brief  		- API that starts, waits, and reads one or more analog conversions

 	 * @param 		- *pADCHandle : contains ADC peripheral base address in MCU memory as well as configuration values
 	 * @param 		- *pRxBuffer : user-application buffer used to read in digitally-converted value from the ADC's data register

 	 * @retval 		- none

 	 * @Note		- This is a blocking function

*/
void ADC_Read(ADC_Handle_t *pADCHandle, uint16_t *pRxBuffer)
{
	//1. Determine what kind of mode the ADC is in

	if( pADCHandle->ADC_Config.ADC_Mode == ADC_SINGLE_CONVERSION_MODE )
	{
		//1.1 Start sampling input voltage
		//If already sampling, something has gone wrong. Issue application event

		if( ADC_GetFlagStatus( pADCHandle->pADCx, ADC_FLAG_STRT ) )
		{
			ADC_ApplicationEventCallBack(pADCHandle, ADC_ERROR_STRT);
		}

		ADC_StartADC( pADCHandle );

		//1.2 If a regular channel was converted, check for end of conversion flag and read data register into buffer

		while( !( ADC_GetFlagStatus( pADCHandle->pADCx, ADC_FLAG_EOC ) ) )
			;

		*(pRxBuffer) = (uint16_t) pADCHandle->pADCx->DR;

		//1.3 Once regular channel has been converted, clear STRT flag
		ADC_ClearFlag(pADCHandle->pADCx, ADC_FLAG_STRT);

	}
	else if( pADCHandle->ADC_Config.ADC_Mode == ADC_CONT_CONVERSION_MODE )
	{
		;
	}
	else if( pADCHandle->ADC_Config.ADC_Mode == ADC_SCAN_CONVERSION_MODE )
	{
		;
	}
	else if( pADCHandle->ADC_Config.ADC_Mode == ADC_DISCONT_CONVERSION_MODE )
	{
		;
	}

}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_IRQConfig

 	 * @brief  		- API that configures interrupts generated by a ADC. This function handles the configurations needed for interrupts on the processor side
 	 * 				- Refer to processor guide here for more details: https://www.engr.scu.edu/~dlewis/book3/docs/Cortex-M4_Devices_Generic_User_Guide.pdf

 	 * @param 		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table
 	 * @param 		- EnOrDi : macros to enable or disable the IRQ (ENABLE or DISABLE macros in MCU specific header file

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
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
	 * @fn			- ADC_IRQPriorityConfig

	 * @brief  		- API that configures the priority level of a given IRQ (ADC-driven interrupt).

	 * @param 		- IRQPriority : Value that contains priority level of interrupt as compared to other interrupts. Similar to ADC_IRQInterruptConfig, this is also handled on the processor side in Cortex-M4 internal peripheral registers
	 * @param		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table

	 * @retval 		- none

	 * @Note		- none

*/
void ADC_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. Find out which IPR register the IRQ is in
	uint8_t Offset = IRQNumber / 4;

	//2. Clear and write by shifting the priority value into the correct position
	uint8_t temp = (IRQNumber % 4) * 8;

	uint8_t shiftAmount = temp + ( 8 - NO_PR_BITS_IMPLEMENTED );															//Note: In the Cortex-M4, only 4 most significant bits of each of the 4 "sections" in the Interrupt priority register (IPR) are implemented, not the last 4. Because if this, you are limited to only 16 priority levels instead of 256 (this was for cost savings apparently). Keeping this in mind, you must also shift whatever priority level desired over to the left by 4 bits no matter the section of the byte it is in.
	*(NVIC_IPR_BASE_ADDR + (Offset) ) &= ~( 0xFF << shiftAmount );														//Clear the existing priority level
	*(NVIC_IPR_BASE_ADDR + (Offset) ) |= ( IRQPriority << shiftAmount );												//Write the desired priority level
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_EnableIT

 	 * @brief  		- API that enables interrupts generated by ADC peripheral.

 	 * @param 		- pADCHandle : Contains ADC peripheral base address in MCU memory as well as other global variables
 	 * @param 		- ConvBuffer : Contains address of user data buffer
 	 * @param 		- length : Contains address of user data buffer

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_EnableIT(ADC_Handle_t *pADCHandle, uint16_t *ConvBuffer, uint8_t Length)
{
	//0. Configure address of global driver buffer to user buffer so that driver has access to change its values
	pADCHandle->pADC_DataBuffer = ConvBuffer;

	//0.5 Copy number of conversions per sequence (length) into global handler that can be seen by interrupt routines
	pADCHandle->ADC_SeqLen = Length;


	//Enable interrupts as non-polling means of alerting program that analog conversion is complete, error has occurred, etc.

	//1. Enable EOCS flag (end of conversion) interrupts
	pADCHandle->pADCx->CR1 |= ( 1 << ADC_CR1_EOCIE );

	//2. Enable Watch dog high and low threshold flag interrupts
	pADCHandle->pADCx->CR1 |= ( 1 << ADC_CR1_AWDIE );

	//3. Enable overrun flag interrupts
	pADCHandle->pADCx->CR1 |= ( 1 << ADC_CR1_OVRIE );


	//5. Start ADC conversion - if already converting, issue a warning
	if( ADC_GetFlagStatus(pADCHandle->pADCx, ADC_FLAG_STRT) )
	{
		ADC_ApplicationEventCallBack(pADCHandle, ADC_ERROR_STRT);
	}

	ADC_StartADC(pADCHandle);
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_DisableIT

 	 * @brief  		- API that disables interrupts generated by ADC peripheral.

 	 * @param 		- pADCHandle : Contains ADC peripheral base address in MCU memory as well as other global variables

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_DisableIT(ADC_Handle_t *pADCHandle)
{
	//0. Reset handle structure in preparation of future data
	pADCHandle->pADC_DataBuffer = NULL;
	pADCHandle->ADC_SeqLen = 0;

	//Disable interrupts

	//1. Disable EOCS flag (end of conversion) interrupts
	pADCHandle->pADCx->CR1 &= ~( 1 << ADC_CR1_EOCIE );

	//2. Disable Watch dog high and low threshold flag interrupts
	pADCHandle->pADCx->CR1 &= ~( 1 << ADC_CR1_AWDIE );

	//3. Disable overrun flag interrupts
	pADCHandle->pADCx->CR1 &= ~( 1 << ADC_CR1_OVRIE );


	//4. Turn off ADC
	ADC_PeripheralOnOffControl( (pADCHandle->pADCx) , DISABLE);
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_IRQHandling

 	 * @brief  		- API that handles interrupts generated by ADC peripheral.

 	 * @param 		- pADCHandle : Contains ADC peripheral base address in MCU memory as well as other global variables

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_IRQHandling(ADC_Handle_t *pADCHandle)
{
	//Temporary variables to make sure interrupts are enabled
	uint8_t temp1 = ( pADCHandle->pADCx->CR1 & ( 1 << ADC_CR1_EOCIE ) );
	uint8_t temp2 = ( pADCHandle->pADCx->CR1 & ( 1 << ADC_CR1_AWDIE ) );
	uint32_t temp3 = ( pADCHandle->pADCx->CR1 & ( 1 << ADC_CR1_OVRIE ) );


	if( ADC_GetFlagStatus(pADCHandle->pADCx, ADC_FLAG_OVR) && temp3 )
	{
		//1. Check if the interrupt was triggered by "overrun" flag

		//1.1 Behavior of an overrun event defined by user
		ADC_ApplicationEventCallBack(pADCHandle, ADC_EVENT_OVR);
	}


	if( ADC_GetFlagStatus(pADCHandle->pADCx, ADC_FLAG_AWD) && temp2 )
	{
		//2. Check if the interrupt was triggered by "analog watch dog threshold" flag

		//2.1 Behavior of an analog watch dog threshold event defined by user
		ADC_HandleRead(pADCHandle);

		ADC_ApplicationEventCallBack(pADCHandle, ADC_EVENT_AWD);
	}


	if( ADC_GetFlagStatus(pADCHandle->pADCx, ADC_FLAG_EOC) && temp1 )
	{
		//3. Check if the interrupt was triggered by "end of conversion" flag

		//3.1 Check if sequence has more than 1 conversions left in it
		if( ( pADCHandle->ADC_SeqLen ) > 1 )
		{
			//3.2 if there is more than 1 conversions left in sequence, continue with conversions

			ADC_HandleRead(pADCHandle);

			//3.2.1 Decrement length of sequence tracker
			pADCHandle->ADC_SeqLen--;
		}
		else if( ( pADCHandle->ADC_SeqLen ) == 1 )
		{
			//3.3 if there is only 1 conversion left in sequence, finish converting last analog value and then turn off interrupts and ADC

			ADC_HandleRead(pADCHandle);

			//3.3.1 Disable interrupts and turn off ADC
			ADC_DisableIT(pADCHandle);

			//The purpose of this is that when the timer interrupt is triggered, it will turn on the ADC and the ADC interrupts.
			//Because this is meant for a non-continuous application, this will save power in the down time
		}

		//NOTE: EOC flag should automatically be cleared by software read of DR

		//Handler for more than 1 conversion - must be implemented in user application
		ADC_ApplicationEventCallBack(pADCHandle, ADC_EVENT_EOC);
	}


}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_StartADC

 	 * @brief  		- Helper API that starts the conversion on the ADC

 	 * @param 		- *pADCHandle : contains ADC peripheral base address in MCU memory as well as configuration values

 	 * @retval 		- none

 	 * @Note		- API only starts conversion of regular channels, not injected ones

*/
void ADC_StartADC(ADC_Handle_t *pADCHandle)
{
	uint8_t temp = ( pADCHandle->pADCx->CR2 & ( 1 << ADC_CR2_ADON ) );

	if( !temp )
	{
		ADC_PeripheralOnOffControl(pADCHandle->pADCx, ENABLE);
	}

	// Set ADC to start converting - if already converting, issue an application event

	if( ADC_GetFlagStatus(pADCHandle->pADCx, ADC_FLAG_STRT) )
	{

		ADC_ApplicationEventCallBack(pADCHandle, ADC_ERROR_STRT);
	}

	pADCHandle->pADCx->CR2 |= ( 1 << ADC_CR2_SWSTART );

	//ADC_CR2_SWSTART is a read-write bit that set by software to start conversion and cleared by hardware as soon as the conversion starts
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_ApplicationEventCallBack

 	 * @brief  		- API that takes specific course of action if an event has happened

 	 * @param 		- *pADCHandle : contains ADC peripheral base address in MCU memory as well as configuration values
 	 * @param 		- AppEvent : argument containing the specific event or condition that has occurred

 	 * @retval 		- none

 	 * @Note		- Weak driver implementation can be overwritten by user for their own purposes

*/
__weak void ADC_ApplicationEventCallBack(ADC_Handle_t *pADCHandle, uint8_t AppEvent)
{
	//This is a weak implementation. The application may overwrite this function.
}



/*-------------------- Driver-specific helper functions definition section --------------------*/


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_SequenceInit

 	 * @brief  		- Helper API that writes the ADC's SQR1-SQR3 registers with the number and order of channels specified by the user

 	 * @param 		- *pADCHandle : contains ADC peripheral base address in MCU memory as well as configuration values

 	 * @retval 		- none

 	 * @Note		- none

*/
void ADC_SequenceInit(ADC_Handle_t *pADCHandle)
{
	//Configures channel conversion sequence
	//Scheme: Use ADC_Seq_Len to determine how many ADC_SQRx bit fields to write into.

	//Clear SQRx registers
	pADCHandle->pADCx->SQR1 = 0;
	pADCHandle->pADCx->SQR2 = 0;
	pADCHandle->pADCx->SQR3 = 0;

	uint8_t Sequence_Length = pADCHandle->ADC_Config.ADC_Seq_Len;
	uint32_t temp = 0;


	while( Sequence_Length > 12 )
	{
		temp |= pADCHandle->ADC_Config.ADC_Seq_Order[Sequence_Length-1] << ( ( Sequence_Length - 13 ) * 5 );
		Sequence_Length--;
	}
	pADCHandle->pADCx->SQR1 |= temp;
	temp = 0;


	while( Sequence_Length > 6 )
	{
		temp |= pADCHandle->ADC_Config.ADC_Seq_Order[Sequence_Length-1] << ( ( Sequence_Length - 7 ) * 5 );
		Sequence_Length--;
	}
	pADCHandle->pADCx->SQR2 |= temp;
	temp = 0;


	while( Sequence_Length > 0 )
	{
		temp |= pADCHandle->ADC_Config.ADC_Seq_Order[Sequence_Length-1] << ( ( Sequence_Length - 1 ) * 5 );
		Sequence_Length--;
	}
	pADCHandle->pADCx->SQR3 |= temp;

}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_SampleTimeInit

 	 * @brief  		- Helper API that writes the ADC's SMPR1-SMPR2 registers with the sampling time of channels specified by the user

 	 * @param 		- *pADCHandle : contains ADC peripheral base address in MCU memory as well as configuration values

 	 * @retval 		- none

 	 * @Note		- none

*/
static void ADC_SampleTimeInit(ADC_Handle_t *pADCHandle)
{
	//Configures conversion sampling time for each channel

	uint8_t len = pADCHandle->ADC_Config.ADC_Seq_Len;
	uint8_t i = 0;
	uint8_t channel_index;
	uint32_t tempreg = 0;

	while( len )
	{
		tempreg = 0;
		channel_index = pADCHandle->ADC_Config.ADC_Seq_Order[i];
		tempreg |= pADCHandle->ADC_Config.ADC_SamplingTime[channel_index];

		if( channel_index > 9 )
		{
			tempreg <<= ( ( channel_index - 10 ) * 3 );
			pADCHandle->pADCx->SMPR1 |= tempreg;

		}
		else
		{
			tempreg <<= ( channel_index * 3 );
			pADCHandle->pADCx->SMPR2 |= tempreg;
		}

	len--;
	i++;

	}
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_tStabDelay

 	 * @brief  		- Helper API that delays execution of ADC software to allow ADC's hardware to stabilize

 	 * @param 		- none

 	 * @retval 		- none

 	 * @Note		- none

*/
static void ADC_tStabDelay(void)
{
	//Use timers to get more precise delay

	uint32_t t = 3000;

	while(t--);
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- ADC_ConversionRead

 	 * @brief  		- Helper API that converts data in DR to accurate data and then stores value in user data buffer
 	 *
 	 * @param 		- pADCHandle : Contains ADC peripheral base address in MCU memory as well as address of user buffer

 	 * @retval 		- none

 	 * @Note		- none

*/
static void ADC_HandleRead(ADC_Handle_t *pADCHandle)
{
	//1. Check resolution & data alignment to mask data register correctly
	uint8_t resolution = ( ( pADCHandle->pADCx->CR1 >> ADC_CR1_RES_1_0 ) & (0x3) );
	uint8_t alignment = ( ( pADCHandle->pADCx->CR1 >> ADC_CR2_ALIGN ) & (0x1) );

	//2. Calculate right-shift amount in case of left aligned bits
	uint8_t shiftAmt = 4 + ( resolution * 2 );

	//2.5. Calculate mask value
	uint16_t maskVal = ( 0xFFF >> (resolution * 2) );


	uint16_t tempreg = 0;

	//3. Check alignment
	if( alignment )
	{
		//4. Left aligned bits

		if( resolution == 3)
		{
			//4.1 Check for special case: 6-bit DR resolution
			//Right shift by 2, then mask the last 6 bits and assign value to user buffer

			tempreg = (uint16_t) ( (pADCHandle->pADCx->DR) >> 0x2 );
			*(pADCHandle->pADC_DataBuffer) = ( tempreg & maskVal );
		}
		else
		{
			//4.2 Normal case: 12, 10, or 8 bit DR resolution
			//Right shift by shiftAmt, then mask appropriate number of bits and assign value to user buffer

			tempreg = (uint16_t) ( (pADCHandle->pADCx->DR) >> shiftAmt );
			*(pADCHandle->pADC_DataBuffer) = ( tempreg & maskVal );
		}
	}
	else
	{
		//5. Right aligned bits

		//5.1 Mask bits appropriately and assign value to user buffer

		tempreg = (uint16_t) (pADCHandle->pADCx->DR);
		*(pADCHandle->pADC_DataBuffer) = ( tempreg & maskVal );
	}

	//Increment user data buffer
	pADCHandle->pADC_DataBuffer++;

}

/*----------------------------------------------------------------------------------------------------*/
