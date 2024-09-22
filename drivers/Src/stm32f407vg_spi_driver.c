/*
 * stm32f407vg_spi_driver.c
 *
 *  Created on: Mar 23, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"

/*
 * Helper functions using in interrupt handling that are not available to user
 * Private APIs that are only accessible by the driver source file
 */

static void SPI_TXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
static void SPI_RXNEIE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
static void SPI_OVR_ERRIE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);

/*
 * Peripheral clock setup
 */

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_PeriClockControl

 	 * @brief  		- enables or disables the clock of a SPI peripheral

 	 * @param 		- *pSPIx : SPI peripheral base address in MCU memory
 	 * @param  		- EnOrDi : macros to enable or disable the clock (ENABLE or DISABLE macros are in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		if( (uint32_t) pSPIx == SPI1_BASE_ADDR )
		{
			SPI1_PCLK_EN();
		}
		else if ( (uint32_t) pSPIx == SPI2_BASE_ADDR )
		{
			SPI2_PCLK_EN();
		}
		else if ( (uint32_t) pSPIx == SPI3_BASE_ADDR )
		{
			SPI3_PCLK_EN();
		}
		else
			;
	}
	else
	{
		if( (uint32_t) pSPIx == SPI1_BASE_ADDR )
		{
			SPI1_PCLK_DI();
		}
		else if ( (uint32_t) pSPIx == SPI2_BASE_ADDR )
		{
			SPI2_PCLK_DI();
		}
		else if ( (uint32_t) pSPIx == SPI3_BASE_ADDR )
		{
			SPI3_PCLK_DI();
		}
		else
			;
	}
}



/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_Init

 	 * @brief  		- API that initializes a specific SPI peripheral with programmer-defined values from the SPI_Config_t structure

 	 * @param 		- *pSPIHandle : contains SPI peripheral base address in MCU memory as well as configuration values

 	 * @retval 		- none

 	 * @Note		- none

*/
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	uint32_t tempreg = 0;

	// enable the SPI peripheral clock
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	// FIRST: Configure the CR1 (CR[0]) register
		// 1. Configure the device mode.
		// Device configured as master will provide serial clock to all devices connected on SPI network. Slave will not produce their own clock and will instead accept the master's clock
	tempreg |= ( pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR );

		// 2. Configure the bus network
	if( pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD ) // Full duplex (in case of simplex Tx only - MISO line is physically disconnected)
	{
		tempreg &= ~( ( 1 << SPI_CR1_BIDI_MODE ) | ( 1 << SPI_CR1_RX_ONLY ) );
	}
	else if( pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD ) //Half duplex
	{
		tempreg |= ( 1 << SPI_CR1_BIDI_MODE );
	}
	else if( pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY ) //Simplex recieve only mode
	{
		tempreg &= ~( 1 << SPI_CR1_BIDI_MODE );
		tempreg |= ( 1 << SPI_CR1_RX_ONLY );
	}
	else ;

		// 3. Configure serial clock speed
	tempreg |= ( pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR );

		// 4. Configure the data frame format
	tempreg |= ( pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF );

		// 5. Configure the clock polarity
	tempreg |= ( pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL );

		// 6. Configure the clock phase
	tempreg |= ( pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA );

		// 7. Configure the NSS pin input software select
	tempreg |= ( pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM );

	pSPIHandle->pSPIx->CR[0] = tempreg;
}



/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_DeInit

 	 * @brief  		- API that writes reset values into all of a SPI peripheral's registers

 	 * @param 		- *pSPIx : SPI peripheral base address in MCU memory

 	 * @retval 		- none

 	 * @Note		- Function utilizes a feature of the RCC peripheral that quickly resets all the registers of a select peripheral. This saves time as you do not need to individually program the API to reset each register 1 by 1

*/
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if( (uint32_t) pSPIx == SPI1_BASE_ADDR )
	{
		SPI1_REG_RESET();
	}
	else if ( (uint32_t) pSPIx == SPI2_BASE_ADDR )
	{
		SPI2_REG_RESET();
	}
	else if ( (uint32_t) pSPIx == SPI3_BASE_ADDR )
	{
		SPI3_REG_RESET();
	}
	else
		;
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_GetFlagStatus

 	 * @brief  		- API that returns that value of a specific flag in the SPI peripheral registers

 	 * @param 		- *pSPIx : SPI peripheral base address in MCU memory
 	 * @param 		- FlagName : Name of the desired flag you want to check the status of

 	 * @retval 		- data that is read from flag - can only be either 1 or 0 (set or reset)

 	 * @Note		- none

*/
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(	pSPIx->SR & FlagName ) return FLAG_SET;
	else return FLAG_RESET;
}



/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_SendData

 	 * @brief  		- Transmits data out of the MCU to the outside world via the SPI peripheral

 	 * @param 		- *pSPIx : SPI peripheral base address in MCU memory
 	 * @param  		- *pTxBuffer : Transmit data buffer address in memory
 	 * @param  		- Len : Length of buffer

 	 * @retval 		- none

 	 * @Note		- This is a blocking function. Until all the bytes of data have been transmitted, this function will block execution of main program

*/
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while( Len != 0 )
	{
		//wait until Tx buffer is empty
		while( ( SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET ) )
			;

		//Check the DFF bit in CR1
		if( ( pSPIx->CR[0] & ( 1 << SPI_CR1_DFF ) ) ) //DFF = 16bit
		{
			pSPIx->DR = *((uint16_t *)pTxBuffer);
			(uint16_t *)pTxBuffer++;
			Len -= 2;
		}
		else //DFF = 8bit
		{
			pSPIx->DR = *pTxBuffer;
			pTxBuffer++;
			Len--;
		}
	}
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_ReceiveData

 	 * @brief  		- Receives data from the outside world to the MCU to via the SPI peripheral

 	 * @param 		- *pSPIx : SPI peripheral base address in MCU memory
 	 * @param  		- *pRxBuffer : Receive data buffer address in memory
 	 * @param  		- Len : Length of buffer
 	 *
 	 * @retval 		- none

 	 * @Note		- none

*/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while( Len != 0 )
	{
		//wait until Rx buffer is NOT empty
		while( ( SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET ) )
			;

		//Check the DFF bit in CR1
		if( ( pSPIx->CR[0] & ( 1 << SPI_CR1_DFF ) ) ) //DFF = 16bit
		{
			*((uint16_t *)pRxBuffer) = pSPIx->DR; //load the data from DR to Rx buffer address
			(uint16_t *)pRxBuffer++;
			Len -= 2;
		}
		else //DFF = 8bit
		{
			*(pRxBuffer) = pSPIx->DR;
			pRxBuffer++;
			Len--;
		}
	}
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_SendDataIT

 	 * @brief  		- Transmits data out of the MCU to the outside world via the SPI peripheral. Uses interrupts to alert the processor that fresh data can be written.

 	 * @param 		- *pSPIHandle : SPI handle structure address in memory
 	 * @param  		- *pTxBuffer : Transmit data buffer address in memory
 	 * @param  		- Len : Length of buffer
 	 *
 	 * @retval 		- none

 	 * @Note		- none

*/
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX) //only execute API if there is not already data that needs to be loaded into the Tx buffer. Exit function if there is a backlog of data.
	{
		//1. Save the Tx buffer address and Len information in some global variables
		pSPIHandle->TxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		//2. Mark the SPI state as busy in transmission so that no other code can take over SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		//3. Enable the TXEIE bit in the CR2 to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR[1] |= ( 1 << SPI_CR2_TXEIE );

		//4. Data transmission will be handled by ISR code
	}

	return state;
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_ReceiveDataIT

 	 * @brief  		- Receives data from the outside world to the MCU to via the SPI peripheral. Uses interrupts to alert the processor that fresh data can be read.

 	 * @param 		- **pSPIHandle : SPI handle structure address in memory
 	 * @param  		- *pRxBuffer : Receive data buffer address in memory
 	 * @param  		- Len : Length of buffer
 	 *
 	 * @retval 		- none

 	 * @Note		- none

*/
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX) //only execute API if the SPI is not currently working on transmitting anything
	{
		//1. Save the Rx buffer address and Len information in some global variables
		pSPIHandle->RxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;

		//2. Mark the SPI state as busy in transmission so that no other code can take over SPI peripheral until transmission is over
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		//3. Enable the RXNEIE bit in the CR2 to get interrupt whenever RXNE flag is set in SR
		pSPIHandle->pSPIx->CR[1] |= ( 1 << SPI_CR2_RXNEIE );

		//4. Data transmission will be handled by ISR code
	}

	return state;
}


/*
 * IRQ configuration and ISR handling
 */

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_IRQInterruptConfig

 	 * @brief  		- API that configures interrupts generated by a SPI peripheral. This function handles the configurations needed for interrupts on the processor side
 	 * 				- Refer to processor guide here for more details: https://www.engr.scu.edu/~dlewis/book3/docs/Cortex-M4_Devices_Generic_User_Guide.pdf

 	 * @param 		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table
 	 * @param 		- EnOrDi : macros to enable or disable the IRQ (ENABLE or DISABLE macros in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
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
	 * @fn			- SPI_IRQPriorityConfig

	 * @brief  		- API that configures the priority level of a given IRQ (SPI-driven interrupt).

	 * @param 		- IRQPriority : Value that contains priority level of interrupt as compared to other interrupts. This is handled on the processor side in Cortex-M4 internal peripheral registers
	 * @param		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table

	 * @retval 		- none

	 * @Note		- none

*/
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
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
 	 * @fn			- SPI_IRQHandling

 	 * @brief  		- API that handles interrupts generated by GPIO pin.

 	 * @param 		- *pSPIHandle : contains SPI peripheral base address in MCU memory as well as configuration values

 	 * @retval 		- none

 	 * @Note		- none

*/
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp1, temp2;

	//Check for TXE interrupt
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_TXE );
	temp2 = pSPIHandle->pSPIx->CR[1] & ( 1 << SPI_CR2_TXEIE );


	if( temp1 && temp2 )
	{
		SPI_TXE_Interrupt_Handle(pSPIHandle);
	}

	//Check for RXNE interrupt
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_RXNE );
	temp2 = pSPIHandle->pSPIx->CR[1] & ( 1 << SPI_CR2_RXNEIE );

	if( temp1 && temp2 )
	{
		SPI_RXNEIE_Interrupt_Handle(pSPIHandle);
	}

	//Check for ERR interrupt
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_OVR );
	temp2 = pSPIHandle->pSPIx->CR[1] & ( 1 << SPI_CR2_ERRIE );

	if( temp1 && temp2 )
	{
		SPI_OVR_ERRIE_Interrupt_Handle(pSPIHandle);
	}

}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_PeripheralControl

 	 * @brief  		- API that enables or disables the SPIx peripheral

 	 * @param 		- *pSPIx : SPI peripheral base address in MCU memory
 	 * @param 		- EnOrDi : User-inputted value to enable or disable the device's SPI peripheral

 	 * @retval 		- none

 	 * @Note		- none

*/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		pSPIx->CR[0] |= (1 << SPI_CR1_SPE);
	}
	else if( EnOrDi == DISABLE )
	{
		pSPIx->CR[0] &= ~(1 << SPI_CR1_SPE);
	}
	else
		;
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_SSIConfig

 	 * @brief  		- API that forces NSS line of the SPI peripheral to either Vcc or GND while in software mode

 	 * @param 		- *pSPIx : SPI peripheral base address in MCU memory
 	 * @param 		- EnOrDi : User-inputted value to enable or disable the device's SPI peripheral

 	 * @retval 		- none

 	 * @Note		- none

*/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		pSPIx->CR[0] |= (1 << SPI_CR1_SSI);
	}
	else if( EnOrDi == DISABLE )
	{
		pSPIx->CR[0] &= ~(1 << SPI_CR1_SSI);
	}
	else
		;
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- SPI_SSOEConfig

 	 * @brief  		- API that drives the NSS line of the SPI peripheral to GND activating the SPI peripherals of the slaves in the network connected to the NSS line of the master

 	 * @param 		- *pSPIx : SPI peripheral base address in MCU memory
 	 * @param 		- EnOrDi : User-inputted value to enable or disable the device's SPI peripheral

 	 * @retval 		- none

 	 * @Note		- Can only be set if device is configured as master, NSS pin is in hardware mode, and bus configuration is not in multi-master mode

*/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		pSPIx->CR[1] |= (1 << SPI_CR2_SSOE);
	}
	else if( EnOrDi == DISABLE )
	{
		pSPIx->CR[1] &= ~(1 << SPI_CR2_SSOE);
	}
	else
		;
}

//Helper function implementations

static void SPI_TXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
	//Check the DFF bit in CR1
	if( ( pSPIHandle->pSPIx->CR[0] & ( 1 << SPI_CR1_DFF ) ) ) //DFF = 16bit
	{
		pSPIHandle->pSPIx->DR = *((uint16_t *)pSPIHandle->TxBuffer);
		(uint16_t *)pSPIHandle->TxBuffer++;
		pSPIHandle->TxLen -= 2;
	}
	else //DFF = 8bit
	{
		pSPIHandle->pSPIx->DR = *(pSPIHandle->TxBuffer);
		pSPIHandle->TxBuffer++;
		pSPIHandle->TxLen--;
	}

	//Check to see if the length of the buffer is 0. Means we are done transmitting data and the Tx buffer is empty.
	if( !pSPIHandle->TxLen )
	{
		//Prevent interrupts from occurring when the TXE flag is set again to avoid sending bad data/wasting processor resources with empty interrupts.
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallBack(pSPIHandle, SPI_EVENT_TX_CMPLT); //inform the application that transmission of data is complete. To be implemented by user application
	}
}

static void SPI_RXNEIE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
	//Check the DFF bit in CR1
	if( ( pSPIHandle->pSPIx->CR[0] & ( 1 << SPI_CR1_DFF ) ) ) //DFF = 16bit
	{
		*((uint16_t *)pSPIHandle->RxBuffer) = (uint16_t) pSPIHandle->pSPIx->DR;
		(uint16_t *)pSPIHandle->RxBuffer++;
		pSPIHandle->RxLen -= 2;
	}
	else //DFF = 8bit
	{
		*(pSPIHandle->RxBuffer) = pSPIHandle->pSPIx->DR;
		pSPIHandle->RxBuffer++;
		pSPIHandle->RxLen--;
	}

	//Check to see if the length of the buffer is 0. Means we are done transmitting data and the Tx buffer is empty.
	if( !pSPIHandle->RxLen )
	{
		//Prevent interrupts from occurring when the RXNE flag is set again to avoid sending bad data/wasting processor resources with empty interrupts.
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallBack(pSPIHandle, SPI_EVENT_RX_CMPLT); //inform the application that transmission of data is complete. To be implemented by user application
	}
}

static void SPI_OVR_ERRIE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
	//1. Clear the OVR flag
		//Refer to RM 28.4.8 on how to clear OVR flag - it requires a read of the DR followed by a read of the SR
		if(pSPIHandle->TxState != SPI_BUSY_IN_TX) //OVR flag is specific to data reception - therefore, action is only taken during that time
		{
			uint32_t dummy = pSPIHandle->pSPIx->DR;
			dummy = pSPIHandle->pSPIx->SR;
			(void)dummy;
		}

	//2. Notify the user application that OVR condition has occurred and data has been lost
		SPI_ApplicationEventCallBack(pSPIHandle, SPI_EVENT_OVR_ERR); //inform the application that API has acknowledged the fault and possibly cleared the OVR flag. To be implemented by user application

}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR[1] &= ~( 1 << SPI_CR2_TXEIE );
	pSPIHandle->TxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR[1] &= ~( 1 << SPI_CR2_RXNEIE );
	pSPIHandle->RxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint32_t dummy = pSPIx->DR;
	dummy = pSPIx->SR;
	(void)dummy;
}

__weak void SPI_ApplicationEventCallBack(SPI_Handle_t *pSPIHandle, uint8_t AppEvent)
{
	//This is a weak implementation. The application may overwrite this function.
}
